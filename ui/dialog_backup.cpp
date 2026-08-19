#include "dialog_backup.h"
#include "ui_dialog_backup.h"

#include "db/Database.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>

DialogBackup::DialogBackup(QWidget *parent) : QDialog(parent), ui(new Ui::DialogBackup) {
    ui->setupUi(this);
    setWindowTitle(tr("Backup"));
}

DialogBackup::~DialogBackup() {
    delete ui;
}

void DialogBackup::on_export_button_clicked() {
    QJsonArray profilesArr;
    for (const auto &[id, profile]: Vload::profileManager->profiles) {
        profilesArr += profile->ToJson();
    }
    QJsonArray groupsArr;
    for (const auto &[id, group]: Vload::profileManager->groups) {
        groupsArr += group->ToJson();
    }

    QJsonObject backup{
        {"version", 1},
        {"profiles", profilesArr},
        {"groups", groupsArr},
        {"settings", Vload::dataStore->ToJson()},
    };

    auto fn = QFileDialog::getSaveFileName(this, tr("Export Backup"), "vload-backup.json", "*.json");
    if (fn.isEmpty()) return;
    if (!fn.endsWith(".json")) fn += ".json";

    QFile f(fn);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        MessageBoxWarning(tr("Export Backup"), f.errorString());
        return;
    }
    f.write(QJsonDocument(backup).toJson(QJsonDocument::Indented));
    f.close();

    QMessageBox::information(this, tr("Export Backup"),
                              tr("Exported %1 profile(s) and %2 group(s).").arg(profilesArr.count()).arg(groupsArr.count()));
}

void DialogBackup::on_import_button_clicked() {
    auto fn = QFileDialog::getOpenFileName(this, tr("Import Backup"), "", "*.json");
    if (fn.isEmpty()) return;

    QFile f(fn);
    if (!f.open(QIODevice::ReadOnly)) {
        MessageBoxWarning(tr("Import Backup"), f.errorString());
        return;
    }
    auto doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    if (!doc.isObject() || doc.object()["version"].toInt() != 1) {
        MessageBoxWarning(tr("Import Backup"), tr("Not a valid vload backup file"));
        return;
    }
    auto root = doc.object();
    auto profilesArr = root["profiles"].toArray();
    auto groupsArr = root["groups"].toArray();
    auto hasSettings = root.contains("settings");

    auto btn = QMessageBox::question(this, tr("Import Backup"),
                                      tr("Found %1 profile(s), %2 group(s)%3.\n\nImport will ADD these alongside your existing "
                                         "data (it will not replace or delete anything). Continue?")
                                          .arg(profilesArr.count())
                                          .arg(groupsArr.count())
                                          .arg(hasSettings ? tr(", and app settings") : ""));
    if (btn != QMessageBox::Yes) return;

    // groups first, so profiles can be assigned to their (re-id'd) group
    QMap<int, int> gidOld2New;
    for (const auto &gv: groupsArr) {
        auto gobj = gv.toObject();
        auto group = Vload::ProfileManager::NewGroup();
        group->FromJson(gobj);
        auto oldId = group->id;
        group->id = -1;
        group->order.clear(); // rebuilt below from the newly re-id'd profiles
        Vload::profileManager->AddGroup(group);
        gidOld2New[oldId] = group->id;
    }

    QMap<int, QList<int>> newGroupOrder; // new gid -> ordered list of newly-allocated profile ids
    int importedProfiles = 0;
    for (const auto &pv: profilesArr) {
        auto pobj = pv.toObject();
        auto ent = Vload::ProfileManager::NewProxyEntity(pobj["type"].toString());
        if (ent->bean->version == -114514) continue; // unknown type, skip
        ent->FromJson(pobj);
        auto oldGid = ent->gid;
        ent->id = -1;
        ent->gid = gidOld2New.value(oldGid, 0); // fall back to Default group if its group wasn't in this backup
        Vload::profileManager->AddProfile(ent, ent->gid);
        newGroupOrder[ent->gid] += ent->id;
        importedProfiles++;
    }

    // The imported groups' "order" arrays referenced the backup's original
    // profile ids, which no longer exist post-import - rebuild them from the
    // newly-allocated ids instead.
    for (auto it = gidOld2New.constBegin(); it != gidOld2New.constEnd(); ++it) {
        auto group = Vload::profileManager->GetGroup(it.value());
        if (group == nullptr) continue;
        group->order = newGroupOrder.value(it.value());
        group->Save();
    }

    if (hasSettings) {
        Vload::dataStore->FromJson(root["settings"].toObject());
        Vload::dataStore->Save();
    }

    QMessageBox::information(this, tr("Import Backup"),
                              tr("Imported %1 profile(s) and %2 group(s). A restart is recommended, especially if settings were imported.")
                                  .arg(importedProfiles)
                                  .arg(gidOld2New.count()));
    accept();
}
