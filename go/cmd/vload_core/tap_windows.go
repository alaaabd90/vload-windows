//go:build windows

package main

import (
	"fmt"
	"net"
	"os/exec"
	"strings"
	"sync"
	"time"

	tunprotocol "github.com/sagernet/sing-box/protocol/tun"
	tun "github.com/sagernet/sing-tun"

	"github.com/songgao/water"
)

func init() {
	tunprotocol.HookWindowsTapAdapter = newTapWindowsTun
}

// tapWindowsTun backs sing-box's tun.WinTun interface with a real
// TAP-Windows (tap-windows6/OpenVPN driver) NDIS adapter instead of
// WinTun. WinTun is a pure Layer-3, driver-only virtual adapter with no
// real Ethernet-adapter semantics - Windows' own Internet Connection
// Sharing / Mobile Hotspot has a long-standing history of refusing to
// share a WinTun-backed connection (or working unreliably when it does),
// since ICS's shared-connection detection was built around real NDIS
// adapters. TAP-Windows presents as exactly that: a normal adapter
// Windows' networking stack (and therefore ICS) recognizes fully,
// while still driving it in the same point-to-point "TUN" sub-mode
// (tap_ioctl_config_tun, see the songgao/water Windows implementation)
// so the actual packet I/O this type does is still plain Layer-3 IP,
// same as WinTun - the driver itself, not this code, answers ARP for
// the adapter's own point-to-point peer.
//
// Scope of this first version: IPv4 only, a single statically-assigned
// address (no DHCP for the adapter itself), and no zero-copy read path
// (ReadPacket copies into a fresh buffer per packet, unlike WinTun's
// native zero-copy - correct but slower under heavy load; acceptable to
// start with, revisit if it's a measured bottleneck).
type tapWindowsTun struct {
	ifce      *water.Interface
	mtu       int
	closeOnce sync.Once
}

func newTapWindowsTun(options tun.Options) (tun.WinTun, error) {
	if len(options.Inet4Address) == 0 {
		return nil, fmt.Errorf("TAP-Windows adapter requires an IPv4 address")
	}
	prefix := options.Inet4Address[0]
	// water's "Network" is the *point-to-point* CIDR the tap-windows6
	// driver uses to answer ARP for its own emulated peer - the local
	// address is ours, everything else in the prefix is the peer side
	// the driver pretends to be. Matches how sing-box already derives a
	// gateway for other platforms' TUN devices (see Inet4GatewayAddr).
	network := prefix.String()

	ifce, err := water.New(water.Config{
		DeviceType: water.TUN,
		PlatformSpecificParams: water.PlatformSpecificParams{
			ComponentID: "tap0901",
			Network:     network,
		},
	})
	if err != nil {
		return nil, fmt.Errorf("open TAP-Windows adapter (driver installed? see https://github.com/OpenVPN/tap-windows6): %w", err)
	}

	t := &tapWindowsTun{ifce: ifce, mtu: int(options.MTU)}
	if err := t.configureWindows(options); err != nil {
		ifce.Close()
		return nil, err
	}
	return t, nil
}

// runNetsh mirrors how every other TAP-Windows-based VPN client
// (OpenVPN included) configures the adapter: the tap-windows6 driver's
// own ioctl only sets up its internal ARP-emulation math (see Network
// above), it does not touch the actual Windows-visible interface
// configuration routing/DNS/ICS depend on - netsh is the standard,
// reliable way to bridge that gap without hand-rolling IP Helper API
// calls for a first version.
func runNetsh(args ...string) error {
	cmd := exec.Command("netsh", args...)
	out, err := cmd.CombinedOutput()
	if err != nil {
		return fmt.Errorf("netsh %s: %w (%s)", strings.Join(args, " "), err, strings.TrimSpace(string(out)))
	}
	return nil
}

func (t *tapWindowsTun) configureWindows(options tun.Options) error {
	name := t.ifce.Name()
	// Positional netsh args (`netsh interface ip set address <name> ...`)
	// misparse when name ends in a digit - which "Local Area Connection
	// 3"-style TAP-Windows names always do, since Windows numbers repeat
	// adapter names to disambiguate. netsh's legacy positional parser can
	// swallow that trailing number as part of the "name" token boundary
	// inconsistently across builds, so every call below uses the
	// unambiguous name="..." keyword form instead.
	nameArg := fmt.Sprintf("name=%s", name)

	// TAP-Windows adapters install administratively disabled - bring it
	// up first, everything else fails silently otherwise.
	if err := runNetsh("interface", "set", "interface", nameArg, "admin=enabled"); err != nil {
		return err
	}

	prefix := options.Inet4Address[0]
	addr := prefix.Addr().String()
	if err := runNetsh("interface", "ip", "set", "address", nameArg, "source=static",
		fmt.Sprintf("addr=%s", addr), fmt.Sprintf("mask=%s", maskFromPrefix(prefix))); err != nil {
		return err
	}

	// netsh returning success doesn't mean the address is bound and
	// bindable yet - Windows applies TAP-Windows adapter IP config
	// asynchronously, and sing-box's own bind retry (stack_system.go,
	// 3 tries / 1s apart) isn't always a long enough window, surfacing
	// as "bind: The requested address is not valid in its context." Poll
	// here, before returning, so by the time sing-box's System stack
	// tries to bind its relay listener to this address it already works.
	if err := waitForBindable(addr, 10*time.Second); err != nil {
		return fmt.Errorf("TAP-Windows adapter address %s never became bindable: %w", addr, err)
	}

	if t.mtu > 0 {
		_ = runNetsh("interface", "ipv4", "set", "subinterface", nameArg,
			fmt.Sprintf("mtu=%d", t.mtu), "store=persistent")
	}

	if len(options.DNSServers) > 0 {
		_ = runNetsh("interface", "ip", "set", "dns", nameArg, "source=static",
			fmt.Sprintf("addr=%s", options.DNSServers[0].String()))
	}

	if options.AutoRoute {
		// A plain default route through the adapter - sing-box's router
		// (not this device) still decides what actually gets sent down
		// it via the process/rule matching already in the profile's
		// config; this route just makes the adapter reachable as "the"
		// default path the same way WinTun's AutoRoute setup would.
		gateway := options.Inet4GatewayAddr()
		if gateway.IsValid() {
			_ = runNetsh("interface", "ip", "add", "route", "prefix=0.0.0.0/0", nameArg,
				fmt.Sprintf("nexthop=%s", gateway.String()), "metric=1")
		}
	}
	return nil
}

// waitForBindable polls by actually attempting the same kind of bind
// sing-box's System stack will perform, instead of guessing a fixed
// delay - the safest way to know Windows has finished applying the
// adapter's IP configuration.
func waitForBindable(addr string, timeout time.Duration) error {
	deadline := time.Now().Add(timeout)
	var lastErr error
	for time.Now().Before(deadline) {
		l, err := net.Listen("tcp4", net.JoinHostPort(addr, "0"))
		if err == nil {
			l.Close()
			return nil
		}
		lastErr = err
		time.Sleep(200 * time.Millisecond)
	}
	return lastErr
}

// maskFromPrefix converts a netip.Prefix's bit length to a dotted-quad
// subnet mask, since netsh's "set address" wants that form, not CIDR.
func maskFromPrefix(prefix interface{ Bits() int }) string {
	bits := prefix.Bits()
	mask := [4]byte{}
	for i := 0; i < bits; i++ {
		mask[i/8] |= 1 << uint(7-i%8)
	}
	return fmt.Sprintf("%d.%d.%d.%d", mask[0], mask[1], mask[2], mask[3])
}

func (t *tapWindowsTun) Read(p []byte) (int, error) {
	return t.ifce.Read(p)
}

func (t *tapWindowsTun) Write(p []byte) (int, error) {
	return t.ifce.Write(p)
}

// ReadPacket is the WinTun-specific zero-copy variant sing-box's tun
// inbound uses when available. Not zero-copy here (see the type's doc
// comment) - allocates a correctly-sized buffer per read instead.
func (t *tapWindowsTun) ReadPacket() ([]byte, func(), error) {
	buffer := make([]byte, t.mtu+64)
	n, err := t.ifce.Read(buffer)
	if err != nil {
		return nil, nil, err
	}
	return buffer[:n], func() {}, nil
}

func (t *tapWindowsTun) Name() (string, error) {
	return t.ifce.Name(), nil
}

func (t *tapWindowsTun) Start() error {
	return nil
}

func (t *tapWindowsTun) Close() error {
	var err error
	t.closeOnce.Do(func() {
		err = t.ifce.Close()
	})
	return err
}

// UpdateRouteOptions re-applies address/route configuration when
// sing-box's router recomputes it (e.g. after a default-interface
// change) - same netsh calls as initial setup, safe to repeat.
func (t *tapWindowsTun) UpdateRouteOptions(options tun.Options) error {
	return t.configureWindows(options)
}
