#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class DialogAssets;
}
QT_END_NAMESPACE

// Mirrors Android's AssetsActivity.kt (geoip.db/geosite.db manager),
// scoped to just the default SagerNet/sing-geoip + SagerNet/sing-geosite
// provider (Android offers 4 alternate providers - not replicated here,
// a user who needs a specific provider's rule set can already drop the
// file in manually, same as before this feature existed) and without
// .xz decompression (SagerNet's own release assets are plain .db files
// already, per AssetsActivity.kt's updateAsset(), so it's unneeded for
// the one provider implemented).
class DialogAssets : public QDialog {
    Q_OBJECT

public:
    explicit DialogAssets(QWidget *parent = nullptr);

    ~DialogAssets() override;

private slots:

    void on_update_geoip_clicked();

    void on_update_geosite_clicked();

    void on_import_button_clicked();

private:
    Ui::DialogAssets *ui;

    void refresh();

    void updateAsset(const QString &fileName, const QString &repo);
};
