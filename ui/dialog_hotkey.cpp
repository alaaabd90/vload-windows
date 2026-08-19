#include "dialog_hotkey.h"
#include "ui_dialog_hotkey.h"

#include "ui/mainwindow_interface.h"

DialogHotkey::DialogHotkey(QWidget *parent) : QDialog(parent), ui(new Ui::DialogHotkey) {
    ui->setupUi(this);
    ui->show_mainwindow->setKeySequence(Vload::dataStore->hotkey_mainwindow);
    ui->show_groups->setKeySequence(Vload::dataStore->hotkey_group);
    ui->show_routes->setKeySequence(Vload::dataStore->hotkey_route);
    ui->system_proxy->setKeySequence(Vload::dataStore->hotkey_system_proxy_menu);
    GetMainWindow()->RegisterHotkey(true);
}

DialogHotkey::~DialogHotkey() {
    if (result() == QDialog::Accepted) {
        Vload::dataStore->hotkey_mainwindow = ui->show_mainwindow->keySequence().toString();
        Vload::dataStore->hotkey_group = ui->show_groups->keySequence().toString();
        Vload::dataStore->hotkey_route = ui->show_routes->keySequence().toString();
        Vload::dataStore->hotkey_system_proxy_menu = ui->system_proxy->keySequence().toString();
        Vload::dataStore->Save();
    }
    GetMainWindow()->RegisterHotkey(false);
    delete ui;
}
