#include "edit_ssh.h"
#include "ui_edit_ssh.h"

#include "fmt/SSHBean.hpp"

#include <QInputDialog>

EditSSH::EditSSH(QWidget *parent) : QWidget(parent), ui(new Ui::EditSSH) {
    ui->setupUi(this);
}

EditSSH::~EditSSH() {
    delete ui;
}

void EditSSH::onStart(std::shared_ptr<NekoGui::ProxyEntity> _ent) {
    this->ent = _ent;
    auto bean = this->ent->SSHBean();

    P_LOAD_STRING(user);
    P_LOAD_STRING(password);
    P_C_LOAD_STRING(privateKey);
    P_LOAD_STRING(privateKeyPassphrase);
    P_C_LOAD_STRING(hostKey);
    P_LOAD_STRING(clientVersion);
}

bool EditSSH::onEnd() {
    auto bean = this->ent->SSHBean();

    P_SAVE_STRING(user);
    P_SAVE_STRING(password);
    P_C_SAVE_STRING(privateKey);
    P_SAVE_STRING(privateKeyPassphrase);
    P_C_SAVE_STRING(hostKey);
    P_SAVE_STRING(clientVersion);
    return true;
}

QList<QPair<QPushButton *, QString>> EditSSH::get_editor_cached() {
    return {
        {ui->privateKey, CACHE.privateKey},
        {ui->hostKey, CACHE.hostKey},
    };
}

void EditSSH::on_privateKey_clicked() {
    bool ok;
    auto txt = QInputDialog::getMultiLineText(this, tr("Private Key"), "", CACHE.privateKey, &ok);
    if (ok) {
        CACHE.privateKey = txt;
        editor_cache_updated();
    }
}

void EditSSH::on_hostKey_clicked() {
    bool ok;
    auto txt = QInputDialog::getMultiLineText(this, tr("Host Key (one per line)"), "", CACHE.hostKey, &ok);
    if (ok) {
        CACHE.hostKey = txt;
        editor_cache_updated();
    }
}
