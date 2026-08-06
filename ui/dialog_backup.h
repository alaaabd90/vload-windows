#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class DialogBackup;
}
QT_END_NAMESPACE

// Mirrors Android's BackupFragment.kt, minus routing rules (lower value,
// skipped to keep scope reasonable) and minus Android's Parcel-blob
// serialization (Windows already stores profiles/groups as plain JSON
// files, so backup is just aggregating those, not designing a new format).
class DialogBackup : public QDialog {
    Q_OBJECT

public:
    explicit DialogBackup(QWidget *parent = nullptr);

    ~DialogBackup() override;

private slots:

    void on_export_button_clicked();

    void on_import_button_clicked();

private:
    Ui::DialogBackup *ui;
};
