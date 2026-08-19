#pragma once

#include "fmt/AbstractBean.hpp"

namespace Vload_fmt {
    // Trojan-Go has no native sing-box outbound - Android's own implementation
    // (fmt/trojan_go/TrojanGoBean.java) doesn't reimplement its wire protocol
    // either, it shells out to an external "trojan-go" plugin binary and routes
    // through it via a local SOCKS listener. This mirrors that: NeedExternal/
    // BuildExternal (fmt/Bean2External.cpp) generate trojan-go's own native JSON
    // config and spawn it as an external process, same as the existing Naive/
    // TUIC/Hysteria2-external-core paths already do.
    class TrojanGoBean : public AbstractBean {
    public:
        QString password = "";
        QString sni = "";
        QString type = "original"; // "original" or "ws"
        QString host = "";
        QString path = "";
        QString encryption = "none"; // "none" or "ss;method:password"
        bool allowInsecure = false;

        TrojanGoBean() : AbstractBean(0) {
            _add(new configItem("password", &password, itemType::string));
            _add(new configItem("sni", &sni, itemType::string));
            _add(new configItem("type", &type, itemType::string));
            _add(new configItem("host", &host, itemType::string));
            _add(new configItem("path", &path, itemType::string));
            _add(new configItem("encryption", &encryption, itemType::string));
            _add(new configItem("allowInsecure", &allowInsecure, itemType::boolean));
        };

        QString DisplayCoreType() override { return "Trojan-Go"; };

        QString DisplayType() override { return "Trojan-Go"; };

        int NeedExternal(bool isFirstProfile) override;

        ExternalBuildResult BuildExternal(int mapping_port, int socks_port, int external_stat) override;
    };
} // namespace Vload_fmt
