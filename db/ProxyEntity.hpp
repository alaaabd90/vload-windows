#pragma once

#include "main/Vload.hpp"
#include "db/traffic/TrafficData.hpp"
#include "fmt/AbstractBean.hpp"

namespace Vload_fmt {
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
}; // namespace Vload_fmt

namespace Vload {
    class ProxyEntity : public JsonStore {
    public:
        QString type;

        int id = -1;
        int gid = 0;
        int latency = 0;
        bool lockedImport = false; // imported from a .vloadp locked profile - suppress type/address, block editing
        std::shared_ptr<Vload_fmt::AbstractBean> bean;
        std::shared_ptr<Vload_traffic::TrafficData> traffic_data = std::make_shared<Vload_traffic::TrafficData>("");

        QString full_test_report;

        ProxyEntity(Vload_fmt::AbstractBean *bean, const QString &type_);

        [[nodiscard]] QString DisplayLatency() const;

        [[nodiscard]] QColor DisplayLatencyColor() const;

        [[nodiscard]] Vload_fmt::ChainBean *ChainBean() const {
            return (Vload_fmt::ChainBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::LoadBalanceBean *LoadBalanceBean() const {
            return (Vload_fmt::LoadBalanceBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::SocksHttpBean *SocksHTTPBean() const {
            return (Vload_fmt::SocksHttpBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::ShadowSocksBean *ShadowSocksBean() const {
            return (Vload_fmt::ShadowSocksBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::VMessBean *VMessBean() const {
            return (Vload_fmt::VMessBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::TrojanVLESSBean *TrojanVLESSBean() const {
            return (Vload_fmt::TrojanVLESSBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::NaiveBean *NaiveBean() const {
            return (Vload_fmt::NaiveBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::QUICBean *QUICBean() const {
            return (Vload_fmt::QUICBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::CustomBean *CustomBean() const {
            return (Vload_fmt::CustomBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::AnyTLSBean *AnyTLSBean() const {
            return (Vload_fmt::AnyTLSBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::ShadowTLSBean *ShadowTLSBean() const {
            return (Vload_fmt::ShadowTLSBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::SSHBean *SSHBean() const {
            return (Vload_fmt::SSHBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::HysteriaBean *HysteriaBean() const {
            return (Vload_fmt::HysteriaBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::WireGuardBean *WireGuardBean() const {
            return (Vload_fmt::WireGuardBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::TrojanGoBean *TrojanGoBean() const {
            return (Vload_fmt::TrojanGoBean *) bean.get();
        };

        [[nodiscard]] Vload_fmt::MieruBean *MieruBean() const {
            return (Vload_fmt::MieruBean *) bean.get();
        };
    };
} // namespace Vload
