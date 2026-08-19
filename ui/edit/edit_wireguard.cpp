#include "edit_wireguard.h"
#include "ui_edit_wireguard.h"

#include "fmt/WireGuardBean.hpp"

#include <QRegularExpressionValidator>

EditWireGuard::EditWireGuard(QWidget *parent) : QWidget(parent), ui(new Ui::EditWireGuard) {
    ui->setupUi(this);
}

EditWireGuard::~EditWireGuard() {
    delete ui;
}

void EditWireGuard::onStart(std::shared_ptr<Vload::ProxyEntity> _ent) {
    this->ent = _ent;
    auto bean = this->ent->WireGuardBean();

    P_LOAD_STRING(localAddress);
    P_LOAD_STRING(privateKey);
    P_LOAD_INT(mtu);
    P_LOAD_STRING(peerPublicKey);
    P_LOAD_STRING(preSharedKey);
    P_LOAD_STRING(allowedIps);
    P_LOAD_INT(persistentKeepalive);
}

bool EditWireGuard::onEnd() {
    auto bean = this->ent->WireGuardBean();

    P_SAVE_STRING(localAddress);
    P_SAVE_STRING(privateKey);
    P_SAVE_INT(mtu);
    P_SAVE_STRING(peerPublicKey);
    P_SAVE_STRING(preSharedKey);
    P_SAVE_STRING(allowedIps);
    P_SAVE_INT(persistentKeepalive);
    return true;
}
