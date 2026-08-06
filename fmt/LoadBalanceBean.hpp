#pragma once

#include "main/NekoGui.hpp"

namespace NekoGui_fmt {
    // A vload load-balance profile: references two other existing profiles by
    // id (slot A / slot B), each pinned to a Windows network adapter with a
    // relative speed weight. Modeled on ChainBean - a profile whose bean just
    // points at other profiles rather than describing a proxy protocol of its
    // own (see db/ConfigBuilder.cpp's BuildLoadBalance).
    class LoadBalanceBean : public AbstractBean {
    public:
        QString slotAAdapter = "";
        int slotAProxyId = -1;
        int slotAWeight = 50;
        int slotAMaxConn = 0; // 0 = unlimited

        QString slotBAdapter = "";
        int slotBProxyId = -1;
        int slotBWeight = 50;
        int slotBMaxConn = 0; // 0 = unlimited

        LoadBalanceBean() : AbstractBean(0) {
            _add(new configItem("slotAAdapter", &slotAAdapter, itemType::string));
            _add(new configItem("slotAProxyId", &slotAProxyId, itemType::integer));
            _add(new configItem("slotAWeight", &slotAWeight, itemType::integer));
            _add(new configItem("slotAMaxConn", &slotAMaxConn, itemType::integer));
            _add(new configItem("slotBAdapter", &slotBAdapter, itemType::string));
            _add(new configItem("slotBProxyId", &slotBProxyId, itemType::integer));
            _add(new configItem("slotBWeight", &slotBWeight, itemType::integer));
            _add(new configItem("slotBMaxConn", &slotBMaxConn, itemType::integer));
        };

        QString DisplayType() override { return QObject::tr("Load Balance"); };

        QString DisplayAddress() override { return ""; };
    };
} // namespace NekoGui_fmt
