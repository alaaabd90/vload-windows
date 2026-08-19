#include "dialog_about.h"
#include "ui_dialog_about.h"

#include "main/Vload_Utils.hpp"

#include <QFile>
#include <QDesktopServices>
#include <QUrl>

DialogAbout::DialogAbout(QWidget *parent) : QDialog(parent), ui(new Ui::DialogAbout) {
    ui->setupUi(this);
    setWindowTitle(tr("About"));

    ui->app_name->setText(software_name + " " + NKR_VERSION);
    ui->app_description->setText(tr("A cross-platform sing-box GUI client."));

    QFile licenseFile(":/vload/LICENSE");
    if (licenseFile.open(QIODevice::ReadOnly)) {
        ui->license_text->setPlainText(QString::fromUtf8(licenseFile.readAll()));
        licenseFile.close();
    }

    connect(ui->github_link, &QPushButton::clicked, this, [] {
        QDesktopServices::openUrl(QUrl("https://github.com/alaaabd90/vload-windows"));
    });
    connect(ui->check_update, &QPushButton::clicked, this, [=] {
        if (on_check_update != nullptr) on_check_update();
    });
    connect(ui->close_button, &QPushButton::clicked, this, &QDialog::accept);
}

DialogAbout::~DialogAbout() {
    delete ui;
}
