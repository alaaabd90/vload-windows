#include "dialog_tools.h"
#include "ui_dialog_tools.h"

#include "ui/dialog_stun_test.h"
#include "ui/dialog_backup.h"
#include "ui/dialog_assets.h"

DialogTools::DialogTools(QWidget *parent) : QDialog(parent), ui(new Ui::DialogTools) {
    ui->setupUi(this);
    setWindowTitle(tr("Tools"));
}

DialogTools::~DialogTools() {
    delete ui;
}

void DialogTools::on_stun_button_clicked() {
    DialogStunTest dialog(this);
    dialog.exec();
}

void DialogTools::on_backup_button_clicked() {
    DialogBackup dialog(this);
    dialog.exec();
}

void DialogTools::on_assets_button_clicked() {
    DialogAssets dialog(this);
    dialog.exec();
}
