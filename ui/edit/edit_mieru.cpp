#include "edit_mieru.h"
#include "ui_edit_mieru.h"

#include "fmt/MieruBean.hpp"

#include <QRegularExpressionValidator>

EditMieru::EditMieru(QWidget *parent) : QWidget(parent), ui(new Ui::EditMieru) {
    ui->setupUi(this);

    auto updateVisibility = [=] {
        auto isUdp = ui->protocol->currentText() == "UDP";
        ui->mtu->setVisible(isUdp);
        ui->mtu_l->setVisible(isUdp);
    };
    connect(ui->protocol, &QComboBox::currentTextChanged, this, updateVisibility);
    updateVisibility();
}

EditMieru::~EditMieru() {
    delete ui;
}

void EditMieru::onStart(std::shared_ptr<NekoGui::ProxyEntity> _ent) {
    this->ent = _ent;
    auto bean = this->ent->MieruBean();

    P_LOAD_COMBO_STRING(protocol);
    P_LOAD_STRING(username);
    P_LOAD_STRING(password);
    P_LOAD_INT(mtu);

    emit ui->protocol->currentTextChanged(ui->protocol->currentText());
}

bool EditMieru::onEnd() {
    auto bean = this->ent->MieruBean();

    P_SAVE_COMBO_STRING(protocol);
    P_SAVE_STRING(username);
    P_SAVE_STRING(password);
    P_SAVE_INT(mtu);
    return true;
}
