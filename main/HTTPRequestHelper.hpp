#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <functional>

namespace Vload_network {
    struct VloadHTTPResponse {
        QString error;
        QByteArray data;
        QList<QPair<QByteArray, QByteArray>> header;
    };

    class NetworkRequestHelper : QObject {
        Q_OBJECT

        explicit NetworkRequestHelper(QObject *parent) : QObject(parent){};

        ~NetworkRequestHelper() override = default;
        ;

    public:
        static VloadHTTPResponse HttpGet(const QUrl &url);

        static QString GetHeader(const QList<QPair<QByteArray, QByteArray>> &header, const QString &name);
    };
} // namespace Vload_network

using namespace Vload_network;
