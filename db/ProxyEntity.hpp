#pragma once

#include "main/NekoGui.hpp"
#include "db/traffic/TrafficData.hpp"
#include "fmt/AbstractBean.hpp"

namespace NekoGui_fmt {
    class SocksHttpBean;

    class ShadowSocksBean;

    class VMessBean;

    class TrojanVLESSBean;

    class NaiveBean;

    class QUICBean;

    class CustomBean;

    class ChainBean;

    class LoadBalanceBean;

    class AnyTLSBean;

    class ShadowTLSBean;

    class SSHBean;

    class HysteriaBean;

    class WireGuardBean;

    class TrojanGoBean;

    class MieruBean;
}; // namespace NekoGui_fmt

namespace NekoGui {
    class ProxyEntity : public JsonStore {
    public:
        QString type;

        int id = -1;
        int gid = 0;
        int latency = 0;
        bool lockedImport = false; // imported from a .vloadp locked profile - suppress type/address, block editing
        std::shared_ptr<NekoGui_fmt::AbstractBean> bean;
        std::shared_ptr<NekoGui_traffic::TrafficData> traffic_data = std::make_shared<NekoGui_traffic::TrafficData>("");

        QString full_test_report;

        ProxyEntity(NekoGui_fmt::AbstractBean *bean, const QString &type_);

        [[nodiscard]] QString DisplayLatency() const;

        [[nodiscard]] QColor DisplayLatencyColor() const;

        [[nodiscard]] NekoGui_fmt::ChainBean *ChainBean() const {
            return (NekoGui_fmt::ChainBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::LoadBalanceBean *LoadBalanceBean() const {
            return (NekoGui_fmt::LoadBalanceBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::SocksHttpBean *SocksHTTPBean() const {
            return (NekoGui_fmt::SocksHttpBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::ShadowSocksBean *ShadowSocksBean() const {
            return (NekoGui_fmt::ShadowSocksBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::VMessBean *VMessBean() const {
            return (NekoGui_fmt::VMessBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::TrojanVLESSBean *TrojanVLESSBean() const {
            return (NekoGui_fmt::TrojanVLESSBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::NaiveBean *NaiveBean() const {
            return (NekoGui_fmt::NaiveBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::QUICBean *QUICBean() const {
            return (NekoGui_fmt::QUICBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::CustomBean *CustomBean() const {
            return (NekoGui_fmt::CustomBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::AnyTLSBean *AnyTLSBean() const {
            return (NekoGui_fmt::AnyTLSBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::ShadowTLSBean *ShadowTLSBean() const {
            return (NekoGui_fmt::ShadowTLSBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::SSHBean *SSHBean() const {
            return (NekoGui_fmt::SSHBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::HysteriaBean *HysteriaBean() const {
            return (NekoGui_fmt::HysteriaBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::WireGuardBean *WireGuardBean() const {
            return (NekoGui_fmt::WireGuardBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::TrojanGoBean *TrojanGoBean() const {
            return (NekoGui_fmt::TrojanGoBean *) bean.get();
        };

        [[nodiscard]] NekoGui_fmt::MieruBean *MieruBean() const {
            return (NekoGui_fmt::MieruBean *) bean.get();
        };
    };
} // namespace NekoGui
