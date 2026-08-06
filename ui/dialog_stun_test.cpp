#include "dialog_stun_test.h"
#include "ui_dialog_stun_test.h"

#include <QUdpSocket>
#include <QEventLoop>
#include <QTimer>
#include <QRandomGenerator>
#include <QHostInfo>
#include <QApplication>

DialogStunTest::DialogStunTest(QWidget *parent) : QDialog(parent), ui(new Ui::DialogStunTest) {
    ui->setupUi(this);
    setWindowTitle(tr("STUN Test"));
    ui->wait_label->setVisible(false);
}

DialogStunTest::~DialogStunTest() {
    delete ui;
}

namespace {
    // RFC 5389 magic cookie, fixed for all STUN messages.
    constexpr quint32 STUN_MAGIC_COOKIE = 0x2112A442;

    // A minimal single-request STUN binding test: no retry/multi-server NAT
    // type classification (RFC 3489's Full Cone/Restricted/Symmetric
    // algorithm), just "what's my externally-visible address" - the
    // practically useful part, matching what Android's single Libcore.
    // stunTest() call effectively reports too.
    struct StunResult {
        bool success = false;
        QString text;
    };

    StunResult DoStunTest(const QString &serverHostPort) {
        auto colonIdx = serverHostPort.lastIndexOf(':');
        if (colonIdx < 0) return {false, QObject::tr("Server must be in host:port format")};
        auto host = serverHostPort.left(colonIdx);
        bool portOk;
        auto port = serverHostPort.mid(colonIdx + 1).toUShort(&portOk);
        if (!portOk || port == 0) return {false, QObject::tr("Invalid port")};

        auto resolved = QHostInfo::fromName(host);
        if (resolved.error() != QHostInfo::NoError || resolved.addresses().isEmpty()) {
            return {false, QObject::tr("DNS resolution failed: %1").arg(resolved.errorString())};
        }
        auto serverAddress = resolved.addresses().first();

        QUdpSocket socket;
        if (!socket.bind()) {
            return {false, QObject::tr("Failed to bind local UDP socket")};
        }

        // Build a bare Binding Request: type=0x0001, length=0, magic cookie,
        // random 12-byte transaction id.
        QByteArray request(20, 0);
        request[0] = 0x00;
        request[1] = 0x01;
        request[2] = 0x00;
        request[3] = 0x00;
        request[4] = char((STUN_MAGIC_COOKIE >> 24) & 0xFF);
        request[5] = char((STUN_MAGIC_COOKIE >> 16) & 0xFF);
        request[6] = char((STUN_MAGIC_COOKIE >> 8) & 0xFF);
        request[7] = char(STUN_MAGIC_COOKIE & 0xFF);
        QByteArray transactionId(12, 0);
        for (int i = 0; i < 12; i++) transactionId[i] = char(QRandomGenerator::global()->bounded(256));
        request.replace(8, 12, transactionId);

        socket.writeDatagram(request, serverAddress, port);

        QEventLoop loop;
        QTimer timeout;
        timeout.setSingleShot(true);
        QObject::connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit);
        QObject::connect(&socket, &QUdpSocket::readyRead, &loop, &QEventLoop::quit);
        timeout.start(5000);
        loop.exec();

        if (!socket.hasPendingDatagrams()) {
            return {false, QObject::tr("No response from STUN server (timed out)")};
        }

        QByteArray response(socket.pendingDatagramSize(), 0);
        socket.readDatagram(response.data(), response.size());

        if (response.size() < 20) return {false, QObject::tr("Malformed STUN response (too short)")};
        quint16 msgType = (quint8(response[0]) << 8) | quint8(response[1]);
        if (msgType != 0x0101) {
            return {false, QObject::tr("Server returned an error response (type 0x%1)").arg(msgType, 4, 16, QChar('0'))};
        }
        if (response.mid(8, 12) != transactionId) {
            return {false, QObject::tr("Transaction ID mismatch (spoofed or stale response?)")};
        }

        int pos = 20;
        while (pos + 4 <= response.size()) {
            quint16 attrType = (quint8(response[pos]) << 8) | quint8(response[pos + 1]);
            quint16 attrLen = (quint8(response[pos + 2]) << 8) | quint8(response[pos + 3]);
            int valueStart = pos + 4;
            if (valueStart + attrLen > response.size()) break;

            // XOR-MAPPED-ADDRESS (0x0020) is preferred; fall back to the
            // older, non-XORed MAPPED-ADDRESS (0x0001) if that's all the
            // server sent.
            if ((attrType == 0x0020 || attrType == 0x0001) && attrLen >= 8) {
                bool xored = attrType == 0x0020;
                quint8 family = quint8(response[valueStart + 1]);
                quint16 rawPort = (quint8(response[valueStart + 2]) << 8) | quint8(response[valueStart + 3]);
                quint16 mappedPort = xored ? (rawPort ^ (STUN_MAGIC_COOKIE >> 16)) : rawPort;

                if (family == 0x01 && attrLen >= 8) { // IPv4
                    quint32 rawAddr = (quint8(response[valueStart + 4]) << 24) |
                                       (quint8(response[valueStart + 5]) << 16) |
                                       (quint8(response[valueStart + 6]) << 8) |
                                       quint8(response[valueStart + 7]);
                    quint32 mappedAddr = xored ? (rawAddr ^ STUN_MAGIC_COOKIE) : rawAddr;
                    QHostAddress addr(mappedAddr);
                    auto localAddr = socket.localAddress();
                    auto natDetected = addr != localAddr;
                    return {true, QObject::tr("External address: %1:%2\nLocal address: %3:%4\n%5")
                                       .arg(addr.toString())
                                       .arg(mappedPort)
                                       .arg(localAddr.toString())
                                       .arg(socket.localPort())
                                       .arg(natDetected ? QObject::tr("NAT detected (external address differs from local)")
                                                         : QObject::tr("No NAT detected (directly reachable)"))};
                }
            }

            pos = valueStart + attrLen;
            if (attrLen % 4 != 0) pos += 4 - (attrLen % 4); // attributes are padded to 4-byte boundaries
        }

        return {false, QObject::tr("Response had no MAPPED-ADDRESS/XOR-MAPPED-ADDRESS attribute")};
    }
} // namespace

void DialogStunTest::on_test_button_clicked() {
    ui->test_button->setEnabled(false);
    ui->wait_label->setVisible(true);
    ui->result_text->setPlainText("");
    QApplication::processEvents();

    auto result = DoStunTest(ui->server_edit->text().trimmed());

    ui->wait_label->setVisible(false);
    ui->test_button->setEnabled(true);
    ui->result_text->setPlainText(result.success ? result.text : QObject::tr("Error: %1").arg(result.text));
}
