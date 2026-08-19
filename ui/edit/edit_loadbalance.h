#pragma once

#include <QWidget>
#include "profile_editor.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class EditLoadBalance;
}
QT_END_NAMESPACE

class EditLoadBalance : public QWidget, public ProfileEditor {
    Q_OBJECT

public:
    explicit EditLoadBalance(QWidget *parent = nullptr);

    ~EditLoadBalance() override;

    void onStart(std::shared_ptr<Vload::ProxyEntity> _ent) override;

    bool onEnd() override;

private:
    Ui::EditLoadBalance *ui;
    std::shared_ptr<Vload::ProxyEntity> ent;

    int slotAProfileId = -1;
    int slotBProfileId = -1;

    void RefreshAdapterList();

    void RefreshSlotLabel(bool slotA);

private slots:

    void on_select_profile_a_clicked();

    void on_select_profile_b_clicked();
};
