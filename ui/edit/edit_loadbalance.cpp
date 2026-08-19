#include "edit_loadbalance.h"
#include "ui_edit_loadbalance.h"

#include "ui/mainwindow_interface.h"

#include "db/Database.hpp"
#include "fmt/LoadBalanceBean.hpp"

#include <QNetworkInterface>

EditLoadBalance::EditLoadBalance(QWidget *parent) : QWidget(parent), ui(new Ui::EditLoadBalance) {
    ui->setupUi(this);
    RefreshAdapterList();
}

EditLoadBalance::~EditLoadBalance() {
    delete ui;
}

void EditLoadBalance::RefreshAdapterList() {
    ui->adapter_a->clear();
    ui->adapter_b->clear();

    for (const auto &iface: QNetworkInterface::allInterfaces()) {
        // skip loopback / not-present adapters, keep everything else visible
        // (including disabled ones) since a slot's network may be plugged in
        // later
        if (iface.flags().testFlag(QNetworkInterface::IsLoopBack)) continue;
        if (iface.humanReadableName().isEmpty()) continue;
        ui->adapter_a->addItem(iface.humanReadableName());
        ui->adapter_b->addItem(iface.humanReadableName());
    }
}

void EditLoadBalance::onStart(std::shared_ptr<Vload::ProxyEntity> _ent) {
    this->ent = _ent;
    auto bean = this->ent->LoadBalanceBean();

    slotAProfileId = bean->slotAProxyId;
    slotBProfileId = bean->slotBProxyId;
    RefreshSlotLabel(true);
    RefreshSlotLabel(false);

    // if the saved adapter isn't in the current list (unplugged / renamed),
    // add it anyway so the existing selection isn't silently lost
    if (!bean->slotAAdapter.isEmpty() && ui->adapter_a->findText(bean->slotAAdapter) < 0) {
        ui->adapter_a->addItem(bean->slotAAdapter);
    }
    if (!bean->slotBAdapter.isEmpty() && ui->adapter_b->findText(bean->slotBAdapter) < 0) {
        ui->adapter_b->addItem(bean->slotBAdapter);
    }
    ui->adapter_a->setCurrentText(bean->slotAAdapter);
    ui->adapter_b->setCurrentText(bean->slotBAdapter);
}

bool EditLoadBalance::onEnd() {
    if (get_edit_text_name().isEmpty()) {
        MessageBoxWarning(software_name, tr("Name cannot be empty."));
        return false;
    }
    if (slotAProfileId < 0 || slotBProfileId < 0) {
        MessageBoxWarning(software_name, tr("Please select a profile for both networks."));
        return false;
    }

    auto bean = this->ent->LoadBalanceBean();
    bean->slotAProxyId = slotAProfileId;
    bean->slotBProxyId = slotBProfileId;
    bean->slotAAdapter = ui->adapter_a->currentText();
    bean->slotBAdapter = ui->adapter_b->currentText();

    return true;
}

void EditLoadBalance::RefreshSlotLabel(bool slotA) {
    auto profileId = slotA ? slotAProfileId : slotBProfileId;
    auto label = slotA ? ui->profile_name_a : ui->profile_name_b;

    if (profileId < 0) {
        label->setText(tr("(no profile selected)"));
        return;
    }
    auto profile = Vload::profileManager->GetProfile(profileId);
    label->setText(profile == nullptr ? tr("(profile not found)") : profile->bean->DisplayTypeAndName());
}

void EditLoadBalance::on_select_profile_a_clicked() {
    get_edit_dialog()->hide();
    GetMainWindow()->start_select_mode(this, [=](int id) {
        get_edit_dialog()->show();
        slotAProfileId = id;
        RefreshSlotLabel(true);
    });
}

void EditLoadBalance::on_select_profile_b_clicked() {
    get_edit_dialog()->hide();
    GetMainWindow()->start_select_mode(this, [=](int id) {
        get_edit_dialog()->show();
        slotBProfileId = id;
        RefreshSlotLabel(false);
    });
}
