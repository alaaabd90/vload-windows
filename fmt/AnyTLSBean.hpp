#pragma once

#include "fmt/AbstractBean.hpp"

namespace NekoGui_fmt {
    class AnyTLSBean : public AbstractBean {
    public:
        QString password = "";

        QString idleSessionCheckInterval = "";
        QString idleSessionTimeout = "";
        int minIdleSession = 0;

        // TLS
        bool allowInsecure = false;
        QString sni = "";
        QString alpn = "";
        QString caText = "";
        bool disableSni = false;

        AnyTLSBean() : AbstractBean(0) {
            _add(new configItem("password", &password, itemType::string));
            _add(new configItem("idleSessionCheckInterval", &idleSessionCheckInterval, itemType::string));
            _add(new configItem("idleSessionTimeout", &idleSessionTimeout, itemType::string));
            _add(new configItem("minIdleSession", &minIdleSession, itemType::integer));
            _add(new configItem("allowInsecure", &allowInsecure, itemType::boolean));
            _add(new configItem("sni", &sni, itemType::string));
            _add(new configItem("alpn", &alpn, itemType::string));
            _add(new configItem("caText", &caText, itemType::string));
            _add(new configItem("disableSni", &disableSni, itemType::boolean));
        };

        QString DisplayType() override { return "AnyTLS"; };

        CoreObjOutboundBuildResult BuildCoreObjSingBox() override;
    };
} // namespace NekoGui_fmt
