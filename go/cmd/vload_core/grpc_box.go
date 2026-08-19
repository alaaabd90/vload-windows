package main

import (
	"context"
	"errors"
	"fmt"

	"grpc_server"
	"grpc_server/gen"

	"github.com/matsuridayo/libneko/neko_common"
	"github.com/matsuridayo/libneko/speedtest"
	box "github.com/sagernet/sing-box"
	"github.com/sagernet/sing-box/boxapi"
	"github.com/sagernet/sing-box/include"
	"github.com/sagernet/sing-box/protocol/group"
	"github.com/sagernet/sing/service"

	"log"

	"github.com/sagernet/sing-box/option"
)

// createBox builds a *box.Box from raw sing-box JSON config, using the
// upstream sing-box "include" registries (everything cmd/sing-box itself
// registers) plus the vload "weighted" outbound group, which include.Context
// doesn't register since it's a vload-only addition to sing-box-vload (see
// protocol/group/weighted.go). Mirrors the Android app's libcore/box.go,
// since cmd/sing-box is package main here and can't be imported directly.
func createBox(configJSON []byte) (*box.Box, context.CancelFunc, error) {
	outboundRegistry := include.OutboundRegistry()
	group.RegisterWeighted(outboundRegistry)

	ctx, cancel := context.WithCancel(context.Background())
	ctx = box.Context(ctx,
		include.InboundRegistry(), outboundRegistry, include.EndpointRegistry(),
		include.DNSTransportRegistry(), include.ServiceRegistry(),
	)
	ctx = service.ContextWithDefaultRegistry(ctx)

	var options option.Options
	err := options.UnmarshalJSONContext(ctx, configJSON)
	if err != nil {
		cancel()
		return nil, nil, fmt.Errorf("decode config: %v", err)
	}

	instance, err := box.New(box.Options{Options: options, Context: ctx})
	if err != nil {
		cancel()
		return nil, nil, fmt.Errorf("create service: %v", err)
	}
	return instance, cancel, nil
}

var v2api *boxapi.SbV2rayServer

// weighted is the running instance's "proxy" outbound, when it's a vload
// load-balance ("weighted") group - set in Start(), cleared in Stop(). Nil
// for any other profile type.
var weighted *group.Weighted

type server struct {
	grpc_server.BaseServer
}

func (s *server) Start(ctx context.Context, in *gen.LoadConfigReq) (out *gen.ErrorResp, _ error) {
	var err error

	defer func() {
		out = &gen.ErrorResp{}
		if err != nil {
			out.Error = err.Error()
			instance = nil
		}
	}()

	if neko_common.Debug {
		log.Println("Start:", in.CoreConfig)
	}

	if instance != nil {
		err = errors.New("instance already started")
		return
	}

	instance, instance_cancel, err = createBox([]byte(in.CoreConfig))

	if instance != nil {
		// V2ray Service
		if in.StatsOutbounds != nil {
			v2api = boxapi.NewSbV2rayServer(option.V2RayStatsServiceOptions{
				Enabled:   true,
				Outbounds: in.StatsOutbounds,
			})
			instance.Router().AppendTracker(v2api.StatsService())
		}

		// vload: keep a handle on the "proxy" outbound if it's a weighted
		// (load-balance) group, so UpdateNetworkAvailability can reach it
		weighted = nil
		if proxyOutbound, ok := instance.Outbound().Outbound("proxy"); ok {
			if w, ok := proxyOutbound.(*group.Weighted); ok {
				weighted = w
			}
		}

		err = instance.Start()
		if err != nil {
			instance.Close()
			instance_cancel()
			instance = nil
			weighted = nil
		}
	}

	return
}

func (s *server) Stop(ctx context.Context, in *gen.EmptyReq) (out *gen.ErrorResp, _ error) {
	var err error

	defer func() {
		out = &gen.ErrorResp{}
		if err != nil {
			out.Error = err.Error()
		}
	}()

	if instance == nil {
		return
	}

	instance_cancel()
	instance.Close()

	instance = nil
	weighted = nil

	return
}

func (s *server) Test(ctx context.Context, in *gen.TestReq) (out *gen.TestResp, _ error) {
	var err error
	out = &gen.TestResp{Ms: 0}

	defer func() {
		if err != nil {
			out.Error = err.Error()
		}
	}()

	if in.Mode == gen.TestMode_UrlTest {
		var i *box.Box
		var cancel context.CancelFunc
		if in.Config != nil {
			// Test instance
			i, cancel, err = createBox([]byte(in.Config.CoreConfig))
			if i != nil {
				defer i.Close()
				defer cancel()
			}
			if err != nil {
				return
			}
			err = i.Start()
			if err != nil {
				return
			}
		} else {
			// Test running instance
			i = instance
			if i == nil {
				return
			}
		}
		// Latency
		out.Ms, err = speedtest.UrlTest(boxapi.CreateProxyHttpClient(i, nil), in.Url, in.Timeout, speedtest.UrlTestStandard_RTT)
	} else if in.Mode == gen.TestMode_TcpPing {
		out.Ms, err = speedtest.TcpPing(in.Address, in.Timeout)
	} else if in.Mode == gen.TestMode_FullTest {
		i, cancel, err := createBox([]byte(in.Config.CoreConfig))
		if i != nil {
			defer i.Close()
			defer cancel()
		}
		if err != nil {
			return
		}
		err = i.Start()
		if err != nil {
			return
		}
		return grpc_server.DoFullTest(ctx, in, i)
	}

	return
}

func (s *server) QueryStats(ctx context.Context, in *gen.QueryStatsReq) (out *gen.QueryStatsResp, _ error) {
	out = &gen.QueryStatsResp{}

	if instance != nil && v2api != nil {
		out.Traffic = v2api.QueryStats(fmt.Sprintf("outbound>>>%s>>>traffic>>>%s", in.Tag, in.Direct))
	}

	return
}

func (s *server) ListConnections(ctx context.Context, in *gen.EmptyReq) (*gen.ListConnectionsResp, error) {
	out := &gen.ListConnectionsResp{
		// TODO upstream api
	}
	return out, nil
}

func (s *server) UpdateNetworkAvailability(ctx context.Context, in *gen.UpdateNetworkAvailabilityReq) (out *gen.ErrorResp, _ error) {
	out = &gen.ErrorResp{}
	if weighted == nil {
		out.Error = "not running a load-balance profile"
		return
	}
	weighted.UpdateAvailability(int(in.Slot), in.Available)
	return
}
