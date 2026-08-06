#include "edit_anytls.h"
#include "ui_edit_anytls.h"

#include "fmt/AnyTLSBean.hpp"

#include <QInputDialog>
#include <QRegularExpressionValidator>

EditAnyTLS::EditAnyTLS(QWidget *parent) : QWidget(parent), ui(new Ui::EditAnyTLS) {
    ui->setupUi(this);
}

EditAnyTLS::~EditAnyTLS() {
    delete ui;
}

void EditAnyTLS::onStart(std::shared_ptr<NekoGui::ProxyEntity> _ent) {
    this->ent = _ent;
    auto bean = this->ent->AnyTLSBean();

    P_LOAD_STRING(password);
    P_LOAD_STRING(idleSessionCheckInterval);
    P_LOAD_STRING(idleSessionTimeout);
    P_LOAD_INT(minIdleSession);

    P_LOAD_STRING(sni);
    P_LOAD_STRING(alpn);
    P_C_LOAD_STRING(caText);
    P_LOAD_BOOL(allowInsecure);
    P_LOAD_BOOL(disableSni);
}

bool EditAnyTLS::onEnd() {
    auto bean = this->ent->AnyTLSBean();

    P_SAVE_STRING(password);
    P_SAVE_STRING(idleSessionCheckInterval);
    P_SAVE_STRING(idleSessionTimeout);
    P_SAVE_INT(minIdleSession);

    P_SAVE_STRING(sni);
    P_SAVE_STRING(alpn);
    P_C_SAVE_STRING(caText);
    P_SAVE_BOOL(allowInsecure);
    P_SAVE_BOOL(disableSni);
    return true;
}

QList<QPair<QPushButton *, QString>> EditAnyTLS::get_editor_cached() {
    return {
        {ui->certificate, CACHE.caText},
    };
}

void EditAnyTLS::on_certificate_clicked() {
    bool ok;
    auto txt = QInputDialog::getMultiLineText(this, tr("Certificate"), "", CACHE.caText, &ok);
    if (ok) {
        CACHE.caText = txt;
        editor_cache_updated();
    }
}
