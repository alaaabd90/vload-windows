#pragma once

#include "AbstractBean.hpp"

namespace Vload_fmt {
    class V2rayStreamSettings : public JsonStore {
    public:
        QString network = "tcp";
        QString security = "";
        QString packet_encoding = "";
        // ws/http/grpc/tcp-http/httpupgrade
        QString path = "";
        QString host = "";
        // kcp/quic/tcp-http
        QString header_type = "";
        // tls
        QString sni = "";
        QString alpn = "";
        QString certificate = "";
        QString utlsFingerprint = "";
        bool allow_insecure = false;
        // ws early data
        QString ws_early_data_name = "";
        int ws_early_data_length = 0;
        // reality
        QString reality_pbk = "";
        QString reality_sid = "";
        QString reality_spx = "";
        // multiplex
        int multiplex_status = 0;
        // dialer-level (not TLS-specific, but there's no other per-profile
        // settings surface to hang it off - matches vload-android's
        // StandardV2RayBean.tcpFastOpen)
        bool tcp_fast_open = false;
        // tls fragment - sing-box only fragments a TLS record if
        // record_fragment is ALSO set, fragment alone has no effect (see
        // vload-android's V2RayFmt.kt comment on this exact gotcha)
        bool tls_fragment = false;
        // ech (Encrypted Client Hello)
        bool enable_ech = false;
        QString ech_config = "";

        V2rayStreamSettings() : JsonStore() {
            _add(new configItem("net", &network, itemType::string));
            _add(new configItem("sec", &security, itemType::string));
            _add(new configItem("pac_enc", &packet_encoding, itemType::string));
            _add(new configItem("path", &path, itemType::string));
            _add(new configItem("host", &host, itemType::string));
            _add(new configItem("sni", &sni, itemType::string));
            _add(new configItem("alpn", &alpn, itemType::string));
            _add(new configItem("cert", &certificate, itemType::string));
            _add(new configItem("insecure", &allow_insecure, itemType::boolean));
            _add(new configItem("h_type", &header_type, itemType::string));
            _add(new configItem("ed_name", &ws_early_data_name, itemType::string));
            _add(new configItem("ed_len", &ws_early_data_length, itemType::integer));
            _add(new configItem("utls", &utlsFingerprint, itemType::string));
            _add(new configItem("pbk", &reality_pbk, itemType::string));
            _add(new configItem("sid", &reality_sid, itemType::string));
            _add(new configItem("spx", &reality_spx, itemType::string));
            _add(new configItem("mux_s", &multiplex_status, itemType::integer));
            _add(new configItem("tfo", &tcp_fast_open, itemType::boolean));
            _add(new configItem("tls_frag", &tls_fragment, itemType::boolean));
            _add(new configItem("ech", &enable_ech, itemType::boolean));
            _add(new configItem("ech_cfg", &ech_config, itemType::string));
        }

        void BuildStreamSettingsSingBox(QJsonObject *outbound);
    };

    inline V2rayStreamSettings *GetStreamSettings(AbstractBean *bean) {
        if (bean == nullptr) return nullptr;
        auto stream_item = bean->_get("stream");
        if (stream_item != nullptr) {
            auto stream_store = (JsonStore *) stream_item->ptr;
            auto stream = (Vload_fmt::V2rayStreamSettings *) stream_store;
            return stream;
        }
        return nullptr;
    }
} // namespace Vload_fmt
