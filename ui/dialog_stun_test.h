#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class DialogStunTest;
}
QT_END_NAMESPACE

// Mirrors Android's StunActivity/NetworkFragment (a single "STUN Test" card
// launching a plain UDP binding-request test). Android delegates the actual
// STUN client to Libcore.stunTest(); Windows implements a minimal RFC 5389
// binding request/response directly (no core/gRPC round-trip needed for
// something this small - see dialog_stun_test.cpp).
class DialogStunTest : public QDialog {
    Q_OBJECT

public:
    explicit DialogStunTest(QWidget *parent = nullptr);

    ~DialogStunTest() override;

private slots:

    void on_test_button_clicked();

private:
    Ui::DialogStunTest *ui;
};
