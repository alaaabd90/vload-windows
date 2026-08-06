#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class DialogTools;
}
QT_END_NAMESPACE

// Android's ToolsFragment is a 2-tab container (STUN test + Backup) - this
// is the same grouping as a simple picker instead of a tab widget, since
// both underlying dialogs (DialogStunTest, DialogBackup) already stand on
// their own.
class DialogTools : public QDialog {
    Q_OBJECT

public:
    explicit DialogTools(QWidget *parent = nullptr);

    ~DialogTools() override;

private slots:

    void on_stun_button_clicked();

    void on_backup_button_clicked();

    void on_assets_button_clicked();

private:
    Ui::DialogTools *ui;
};
