#pragma once

#include "fmt/AbstractBean.hpp"

namespace NekoGui_fmt {
    // sing-box 1.13 models WireGuard as a config-level "endpoint", not a plain
    // outbound (see include/wireguard.go upstream - the legacy outbound was
    // removed in 1.13.0). BuildCoreObjSingBox() still returns the JSON object
    // via CoreObjOutboundBuildResult::outbound for pattern-consistency with
    // every other Bean, but db/ConfigBuilder.cpp special-cases type=="wireguard"
    // to divert that object into the config's "endpoints" array instead of
    // "outbounds".
    class WireGuardBean : public AbstractBean {
    public:
        // local interface
        QString localAddress = "172.16.0.2/32";
        QString privateKey = "";
        int mtu = 1408;

        // peer (serverAddress/serverPort from AbstractBean are the peer's endpoint)
        QString peerPublicKey = "";
        QString preSharedKey = "";
        QString allowedIps = "0.0.0.0/0,::/0";
        int persistentKeepalive = 0;

        WireGuardBean() : AbstractBean(0) {
            _add(new configItem("localAddress", &localAddress, itemType::string));
            _add(new configItem("privateKey", &privateKey, itemType::string));
            _add(new configItem("mtu", &mtu, itemType::integer));
            _add(new configItem("peerPublicKey", &peerPublicKey, itemType::string));
            _add(new configItem("preSharedKey", &preSharedKey, itemType::string));
            _add(new configItem("allowedIps", &allowedIps, itemType::string));
            _add(new configItem("persistentKeepalive", &persistentKeepalive, itemType::integer));
        };

        QString DisplayType() override { return "WireGuard"; };

        CoreObjOutboundBuildResult BuildCoreObjSingBox() override;
    };
} // namespace NekoGui_fmt
