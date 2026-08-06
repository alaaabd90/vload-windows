#pragma once

#include <QWidget>
#include "profile_editor.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class EditShadowTLS;
}
QT_END_NAMESPACE

class EditShadowTLS : public QWidget, public ProfileEditor {
    Q_OBJECT

public:
    explicit EditShadowTLS(QWidget *parent = nullptr);

    ~EditShadowTLS() override;

    void onStart(std::shared_ptr<NekoGui::ProxyEntity> _ent) override;

    bool onEnd() override;

    QList<QPair<QPushButton *, QString>> get_editor_cached() override;

private:
    Ui::EditShadowTLS *ui;
    std::shared_ptr<NekoGui::ProxyEntity> ent;

    struct {
        QString caText;
    } CACHE;

private slots:

    void on_certificate_clicked();
};
