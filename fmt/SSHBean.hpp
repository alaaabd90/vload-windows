#pragma once

#include "fmt/AbstractBean.hpp"

namespace Vload_fmt {
    class SSHBean : public AbstractBean {
    public:
        QString user = "root";
        QString password = "";
        QString privateKey = "";
        QString privateKeyPassphrase = "";
        QString hostKey = "";
        QString clientVersion = "";

        SSHBean() : AbstractBean(0) {
            _add(new configItem("user", &user, itemType::string));
            _add(new configItem("password", &password, itemType::string));
            _add(new configItem("privateKey", &privateKey, itemType::string));
            _add(new configItem("privateKeyPassphrase", &privateKeyPassphrase, itemType::string));
            _add(new configItem("hostKey", &hostKey, itemType::string));
            _add(new configItem("clientVersion", &clientVersion, itemType::string));
        };

        QString DisplayType() override { return "SSH"; };

        CoreObjOutboundBuildResult BuildCoreObjSingBox() override;
    };
} // namespace Vload_fmt
