#pragma once

#include <QDialog>
#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui {
    class DialogAbout;
}
QT_END_NAMESPACE

// Mirrors Android's AboutFragment.kt: version, project links, license text,
// and a "Check for Update" action. Android's update-check logic (GitHub
// releases API parsing) isn't reimplemented here - Windows already has a
// more capable gRPC-based update mechanism (MainWindow::CheckUpdate()), so
// this dialog just triggers that via a callback instead of duplicating it.
class DialogAbout : public QDialog {
    Q_OBJECT

public:
    explicit DialogAbout(QWidget *parent = nullptr);

    ~DialogAbout() override;

    std::function<void()> on_check_update;

private:
    Ui::DialogAbout *ui;
};
