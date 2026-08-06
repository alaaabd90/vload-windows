#include "dialog_assets.h"
#include "ui_dialog_assets.h"

#include "db/Database.hpp"
#include "main/HTTPRequestHelper.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include <QApplication>
#include <QDir>

namespace {
    QString AssetDir() {
        return QApplication::applicationDirPath();
    }

    QString VersionFilePath(const QString &fileName) {
        return AssetDir() + "/" + fileName + ".version";
    }

    QString CurrentVersion(const QString &fileName) {
        QFile f(VersionFilePath(fileName));
        if (!f.open(QIODevice::ReadOnly)) return {};
        auto v = QString::fromUtf8(f.readAll()).trimmed();
        f.close();
        return v;
    }
} // namespace

DialogAssets::DialogAssets(QWidget *parent) : QDialog(parent), ui(new Ui::DialogAssets) {
    ui->setupUi(this);
    setWindowTitle(tr("Route Assets"));
    refresh();
}

DialogAssets::~DialogAssets() {
    delete ui;
}

void DialogAssets::refresh() {
    auto geoipInstalled = QFileInfo::exists(AssetDir() + "/geoip.db");
    auto geoipVersion = CurrentVersion("geoip.db");
    ui->geoip_status->setText(geoipInstalled ? (geoipVersion.isEmpty() ? tr("Installed (unknown version)") : geoipVersion)
                                              : tr("Not installed"));

    auto geositeInstalled = QFileInfo::exists(AssetDir() + "/geosite.db");
    auto geositeVersion = CurrentVersion("geosite.db");
    ui->geosite_status->setText(geositeInstalled ? (geositeVersion.isEmpty() ? tr("Installed (unknown version)") : geositeVersion)
                                                  : tr("Not installed"));
}

void DialogAssets::on_update_geoip_clicked() {
    updateAsset("geoip.db", "SagerNet/sing-geoip");
}

void DialogAssets::on_update_geosite_clicked() {
    updateAsset("geosite.db", "SagerNet/sing-geosite");
}

void DialogAssets::updateAsset(const QString &fileName, const QString &repo) {
    setEnabled(false);
    QApplication::processEvents();

    auto releaseResp = NetworkRequestHelper::HttpGet(QUrl(QStringLiteral("https://api.github.com/repos/%1/releases/latest").arg(repo)));
    if (!releaseResp.error.isEmpty()) {
        setEnabled(true);
        MessageBoxWarning(tr("Update Asset"), releaseResp.error);
        return;
    }

    auto release = QJsonDocument::fromJson(releaseResp.data).object();
    auto tagName = release["tag_name"].toString();
    if (tagName.isEmpty()) {
        setEnabled(true);
        MessageBoxWarning(tr("Update Asset"), tr("Malformed release response from GitHub"));
        return;
    }
    if (tagName == CurrentVersion(fileName)) {
        setEnabled(true);
        QMessageBox::information(this, tr("Update Asset"), tr("%1 is already up to date (%2).").arg(fileName, tagName));
        return;
    }

    QString downloadUrl;
    for (const auto &assetVal: release["assets"].toArray()) {
        auto assetObj = assetVal.toObject();
        if (assetObj["name"].toString() == fileName) {
            downloadUrl = assetObj["browser_download_url"].toString();
            break;
        }
    }
    if (downloadUrl.isEmpty()) {
        setEnabled(true);
        MessageBoxWarning(tr("Update Asset"), tr("Release %1 has no asset named %2").arg(tagName, fileName));
        return;
    }

    auto downloadResp = NetworkRequestHelper::HttpGet(QUrl(downloadUrl));
    if (!downloadResp.error.isEmpty()) {
        setEnabled(true);
        MessageBoxWarning(tr("Update Asset"), downloadResp.error);
        return;
    }

    QFile f(AssetDir() + "/" + fileName);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        setEnabled(true);
        MessageBoxWarning(tr("Update Asset"), f.errorString());
        return;
    }
    f.write(downloadResp.data);
    f.close();

    QFile vf(VersionFilePath(fileName));
    if (vf.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        vf.write(tagName.toUtf8());
        vf.close();
    }

    setEnabled(true);
    refresh();
    QMessageBox::information(this, tr("Update Asset"), tr("%1 updated to %2.").arg(fileName, tagName));
}

void DialogAssets::on_import_button_clicked() {
    auto fn = QFileDialog::getOpenFileName(this, tr("Import Custom Asset"), "", "*.db");
    if (fn.isEmpty()) return;

    QFileInfo fi(fn);
    auto destPath = AssetDir() + "/" + fi.fileName();
    if (QFile::exists(destPath)) QFile::remove(destPath);
    if (!QFile::copy(fn, destPath)) {
        MessageBoxWarning(tr("Import Custom Asset"), tr("Failed to copy file"));
        return;
    }
    refresh();
    QMessageBox::information(this, tr("Import Custom Asset"), tr("Imported %1.").arg(fi.fileName()));
}
