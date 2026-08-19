#include "edit_shadowtls.h"
#include "ui_edit_shadowtls.h"

#include "fmt/ShadowTLSBean.hpp"

#include <QInputDialog>

EditShadowTLS::EditShadowTLS(QWidget *parent) : QWidget(parent), ui(new Ui::EditShadowTLS) {
    ui->setupUi(this);
}

EditShadowTLS::~EditShadowTLS() {
    delete ui;
}

void EditShadowTLS::onStart(std::shared_ptr<Vload::ProxyEntity> _ent) {
    this->ent = _ent;
    auto bean = this->ent->ShadowTLSBean();

    ui->version->setCurrentText(QString::number(bean->version));
    P_LOAD_STRING(password);

    P_LOAD_STRING(sni);
    P_LOAD_STRING(alpn);
    P_C_LOAD_STRING(caText);
    P_LOAD_BOOL(allowInsecure);
    P_LOAD_BOOL(disableSni);
}

bool EditShadowTLS::onEnd() {
    auto bean = this->ent->ShadowTLSBean();

    bean->version = ui->version->currentText().toInt();
    P_SAVE_STRING(password);

    P_SAVE_STRING(sni);
    P_SAVE_STRING(alpn);
    P_C_SAVE_STRING(caText);
    P_SAVE_BOOL(allowInsecure);
    P_SAVE_BOOL(disableSni);
    return true;
}

QList<QPair<QPushButton *, QString>> EditShadowTLS::get_editor_cached() {
    return {
        {ui->certificate, CACHE.caText},
    };
}

void EditShadowTLS::on_certificate_clicked() {
    bool ok;
    auto txt = QInputDialog::getMultiLineText(this, tr("Certificate"), "", CACHE.caText, &ok);
    if (ok) {
        CACHE.caText = txt;
        editor_cache_updated();
    }
}
