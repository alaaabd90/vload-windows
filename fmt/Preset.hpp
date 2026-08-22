#pragma once

namespace Preset {
    namespace SingBox {
        // Order must match the "Stack" QComboBox items in
        // ui/dialog_vpn_settings.ui (Mixed, gVisor, System, TAP) - the combo
        // only persists an integer index, mapped through this list to build
        // the sing-box "stack" value. These were out of sync (list was
        // gvisor/system/mixed while the UI showed Mixed/gVisor/System),
        // silently running gVisor - the least stable stack on Windows -
        // whenever the UI showed "Mixed" (including the default index 0).
        //
        // Index VpnImplementationTapIndex ("TAP") isn't a real sing-box
        // stack value - it maps to "system" here, not gVisor, and
        // separately tells ConfigBuilder to back the interface with a
        // TAP-Windows adapter instead of WinTun (see windows_tap_adapter
        // in ConfigBuilder.cpp and go/cmd/vload_core/tap_windows.go).
        // Must be "system", not gVisor: gVisor's Windows stack
        // (sing-tun's stack_gvisor.go NewGVisor) requires the tun.Tun
        // implementation to also satisfy GVisorTun (WritePacket taking a
        // gVisor-specific *stack.PacketBuffer, plus NewEndpoint) - an
        // extra, WinTun-specific zero-copy fast path our TAP device
        // doesn't implement. Picking gVisor here failed outright at
        // startup ("gVisor stack is unsupported on current platform").
        // "system" only needs the base tun.Tun interface (plain
        // io.ReadWriter), which the TAP device already satisfies.
        inline QStringList VpnImplementation = {"mixed", "gvisor", "system", "system"};
        inline constexpr int VpnImplementationTapIndex = 3;
        inline QStringList DomainStrategy = {"", "ipv4_only", "ipv6_only", "prefer_ipv4", "prefer_ipv6"};
        inline QStringList UtlsFingerPrint = {"", "chrome", "firefox", "edge", "safari", "360", "qq", "ios", "android", "random", "randomized"};
        inline QStringList ShadowsocksMethods = {"2022-blake3-aes-128-gcm", "2022-blake3-aes-256-gcm", "2022-blake3-chacha20-poly1305", "none", "aes-128-gcm", "aes-192-gcm", "aes-256-gcm", "chacha20-ietf-poly1305", "xchacha20-ietf-poly1305", "aes-128-ctr", "aes-192-ctr", "aes-256-ctr", "aes-128-cfb", "aes-192-cfb", "aes-256-cfb", "rc4-md5", "chacha20-ietf", "xchacha20"};
        inline QStringList Flows = {"xtls-rprx-vision"};
    } // namespace SingBox

    namespace Windows {
        inline QStringList system_proxy_format{"{ip}:{http_port}",
                                               "socks={ip}:{socks_port}",
                                               "http={ip}:{http_port};https={ip}:{http_port};ftp={ip}:{http_port};socks={ip}:{socks_port}",
                                               "http=http://{ip}:{http_port};https=http://{ip}:{http_port}"};
    } // namespace Windows
} // namespace Preset
