#pragma once

#include "fmt/AbstractBean.hpp"

namespace Vload_fmt {
    class ShadowTLSBean : public AbstractBean {
    public:
        int version = 3;
        QString password = "";

        // TLS (the wrapped/faked handshake domain)
        bool allowInsecure = false;
        QString sni = "";
        QString alpn = "";
        QString caText = "";
        bool disableSni = false;

        ShadowTLSBean() : AbstractBean(0) {
            _add(new configItem("version", &version, itemType::integer));
            _add(new configItem("password", &password, itemType::string));
            _add(new configItem("allowInsecure", &allowInsecure, itemType::boolean));
            _add(new configItem("sni", &sni, itemType::string));
            _add(new configItem("alpn", &alpn, itemType::string));
            _add(new configItem("caText", &caText, itemType::string));
            _add(new configItem("disableSni", &disableSni, itemType::boolean));
        };

        QString DisplayType() override { return "ShadowTLS"; };

        CoreObjOutboundBuildResult BuildCoreObjSingBox() override;
    };
} // namespace Vload_fmt
