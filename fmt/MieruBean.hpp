#pragma once

#include "fmt/AbstractBean.hpp"

namespace Vload_fmt {
    // Mieru, like Trojan-Go, has no native sing-box outbound. Android's own
    // implementation (fmt/mieru/MieruBean.java) shells out to an external
    // "mieru-client" plugin binary and routes through it via a local SOCKS
    // listener - see fmt/Bean2External.cpp for the process-spawn side.
    class MieruBean : public AbstractBean {
    public:
        QString protocol = "TCP"; // "TCP" or "UDP"
        QString username = "";
        QString password = "";
        int mtu = 1400; // only used when protocol == "UDP"

        MieruBean() : AbstractBean(0) {
            _add(new configItem("protocol", &protocol, itemType::string));
            _add(new configItem("username", &username, itemType::string));
            _add(new configItem("password", &password, itemType::string));
            _add(new configItem("mtu", &mtu, itemType::integer));
        };

        QString DisplayCoreType() override { return "Mieru"; };

        QString DisplayType() override { return "Mieru"; };

        int NeedExternal(bool isFirstProfile) override;

        ExternalBuildResult BuildExternal(int mapping_port, int socks_port, int external_stat) override;
    };
} // namespace Vload_fmt
