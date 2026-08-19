#pragma once

#include <QWidget>
#include "profile_editor.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class EditTrojanGo;
}
QT_END_NAMESPACE

class EditTrojanGo : public QWidget, public ProfileEditor {
    Q_OBJECT

public:
    explicit EditTrojanGo(QWidget *parent = nullptr);

    ~EditTrojanGo() override;

    void onStart(std::shared_ptr<Vload::ProxyEntity> _ent) override;

    bool onEnd() override;

private:
    Ui::EditTrojanGo *ui;
    std::shared_ptr<Vload::ProxyEntity> ent;
};
