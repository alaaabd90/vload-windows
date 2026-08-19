#include "edit_trojan_go.h"
#include "ui_edit_trojan_go.h"

#include "fmt/TrojanGoBean.hpp"

EditTrojanGo::EditTrojanGo(QWidget *parent) : QWidget(parent), ui(new Ui::EditTrojanGo) {
    ui->setupUi(this);

    auto updateVisibility = [=] {
        auto isWs = ui->type->currentText() == "ws";
        ui->host->setVisible(isWs);
        ui->host_l->setVisible(isWs);
        ui->path->setVisible(isWs);
        ui->path_l->setVisible(isWs);

        auto hasSS = ui->ss_method->currentText() != "None";
        ui->ss_password->setVisible(hasSS);
        ui->ss_password_l->setVisible(hasSS);
    };
    connect(ui->type, &QComboBox::currentTextChanged, this, updateVisibility);
    connect(ui->ss_method, &QComboBox::currentTextChanged, this, updateVisibility);
    updateVisibility();
}

EditTrojanGo::~EditTrojanGo() {
    delete ui;
}

void EditTrojanGo::onStart(std::shared_ptr<Vload::ProxyEntity> _ent) {
    this->ent = _ent;
    auto bean = this->ent->TrojanGoBean();

    P_LOAD_STRING(password);
    P_LOAD_STRING(sni);
    P_LOAD_COMBO_STRING(type);
    P_LOAD_STRING(host);
    P_LOAD_STRING(path);
    P_LOAD_BOOL(allowInsecure);

    if (bean->encryption.startsWith("ss;")) {
        auto rest = SubStrAfter(bean->encryption, ";");
        ui->ss_method->setCurrentText(SubStrBefore(rest, ":"));
        ui->ss_password->setText(SubStrAfter(rest, ":"));
    } else {
        ui->ss_method->setCurrentText("None");
        ui->ss_password->setText("");
    }

    emit ui->type->currentTextChanged(ui->type->currentText());
}

bool EditTrojanGo::onEnd() {
    auto bean = this->ent->TrojanGoBean();

    P_SAVE_STRING(password);
    P_SAVE_STRING(sni);
    P_SAVE_COMBO_STRING(type);
    P_SAVE_STRING(host);
    P_SAVE_STRING(path);
    P_SAVE_BOOL(allowInsecure);

    if (ui->ss_method->currentText() == "None") {
        bean->encryption = "none";
    } else {
        bean->encryption = "ss;" + ui->ss_method->currentText() + ":" + ui->ss_password->text();
    }

    return true;
}
