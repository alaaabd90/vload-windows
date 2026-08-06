#pragma once

#include "fmt/AbstractBean.hpp"

namespace NekoGui_fmt {
    // Hysteria v1 (distinct from Hysteria2, which is QUICBean::proxy_Hysteria2)
    class HysteriaBean : public AbstractBean {
    public:
        QString authString = "";
        QString obfs = "";
        int uploadMbps = 0;
        int downloadMbps = 0;
        QString serverPorts = ""; // port hopping range, e.g. "1000-2000"
        int hopInterval = 10;
        bool disableMtuDiscovery = false;

        // TLS
        bool allowInsecure = false;
        QString sni = "";
        QString alpn = "";
        QString caText = "";
        bool disableSni = false;

        HysteriaBean() : AbstractBean(0) {
            _add(new configItem("authString", &authString, itemType::string));
            _add(new configItem("obfs", &obfs, itemType::string));
            _add(new configItem("uploadMbps", &uploadMbps, itemType::integer));
            _add(new configItem("downloadMbps", &downloadMbps, itemType::integer));
            _add(new configItem("serverPorts", &serverPorts, itemType::string));
            _add(new configItem("hopInterval", &hopInterval, itemType::integer));
            _add(new configItem("disableMtuDiscovery", &disableMtuDiscovery, itemType::boolean));
            _add(new configItem("allowInsecure", &allowInsecure, itemType::boolean));
            _add(new configItem("sni", &sni, itemType::string));
            _add(new configItem("alpn", &alpn, itemType::string));
            _add(new configItem("caText", &caText, itemType::string));
            _add(new configItem("disableSni", &disableSni, itemType::boolean));
        };

        QString DisplayType() override { return "Hysteria"; };

        CoreObjOutboundBuildResult BuildCoreObjSingBox() override;
    };
} // namespace NekoGui_fmt
