#pragma once

#include "ProxyEntity.hpp"
#include "sys/ExternalProcess.hpp"

namespace Vload {
    class BuildConfigResult {
    public:
        QString error;
        QJsonObject coreConfig;

        QList<std::shared_ptr<Vload_traffic::TrafficData>> outboundStats; // all, but not including "bypass" "block"
        std::shared_ptr<Vload_traffic::TrafficData> outboundStat;         // main
        QStringList ignoreConnTag;

        std::list<std::shared_ptr<Vload_fmt::ExternalBuildResult>> extRs;
    };

    class BuildConfigStatus {
    public:
        std::shared_ptr<BuildConfigResult> result;
        std::shared_ptr<ProxyEntity> ent;
        bool forTest;
        bool forExport;

        // priv
        QList<int> globalProfiles;

        // xxList is V2Ray format string list

        QStringList domainListDNSRemote;
        QStringList domainListDNSDirect;
        QStringList domainListRemote;
        QStringList domainListDirect;
        QStringList ipListRemote;
        QStringList ipListDirect;
        QStringList domainListBlock;
        QStringList ipListBlock;

        // config format

        QJsonArray routingRules;
        QJsonArray inbounds;
        QJsonArray outbounds;
        QJsonArray endpoints; // sing-box config-level "endpoints" (currently: WireGuard only)

        // Set by BuildLoadBalance to a priority-mode weighted outbound tag
        // (see sing-box-vload's protocol/group/weighted.go) when the
        // profile is a load-balance one; empty otherwise, in which case
        // dns-remote just detours through the normal tagProxy like every
        // other profile type. DNS wants one consistent network with
        // failover, not both slots' answers mixed per query - see
        // BuildLoadBalance's own comment for why.
        QString dnsProxyTag;

        // Same idea as dnsProxyTag, applied to QUIC (HTTP/3) traffic - see
        // BuildLoadBalance's own comment on the "quic-proxy" outbound for
        // why. Empty when the profile isn't load-balance.
        QString quicProxyTag;
    };

    std::shared_ptr<BuildConfigResult> BuildConfig(const std::shared_ptr<ProxyEntity> &ent, bool forTest, bool forExport);

    void BuildConfigSingBox(const std::shared_ptr<BuildConfigStatus> &status);

    QString BuildChain(int chainId, const std::shared_ptr<BuildConfigStatus> &status);

    QString BuildLoadBalance(const std::shared_ptr<BuildConfigStatus> &status);

    QString BuildChainInternal(int chainId, const QList<std::shared_ptr<ProxyEntity>> &ents,
                               const std::shared_ptr<BuildConfigStatus> &status);

    QString WriteVPNSingBoxConfig();

    QString WriteVPNLinuxScript(const QString &configPath);
} // namespace Vload
