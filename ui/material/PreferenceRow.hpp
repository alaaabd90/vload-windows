#pragma once

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>

// Mirrors the look of Android's androidx.preference.Preference row (used by
// PreferenceFragmentCompat throughout the Android app's settings/editor
// screens - see e.g. StandardV2RaySettingsActivity.kt): bold title, a
// smaller grey summary line underneath, and a trailing control (line edit,
// combo box, checkbox, spin box - whatever the specific setting needs).
// This is the building block every settings/editor screen in this port is
// built from, replacing the old Qt Designer label+field grid dialogs.
class PreferenceRow : public QWidget {
    Q_OBJECT
public:
    explicit PreferenceRow(const QString &title, const QString &summary = "", QWidget *parent = nullptr)
        : QWidget(parent) {
        auto *outer = new QHBoxLayout(this);
        outer->setContentsMargins(16, 10, 16, 10);
        outer->setSpacing(12);

        auto *textCol = new QVBoxLayout();
        textCol->setSpacing(2);

        titleLabel = new QLabel(title, this);
        titleLabel->setObjectName("prefRowTitle");
        titleLabel->setStyleSheet("font-weight: 600; font-size: 14px;");
        textCol->addWidget(titleLabel);

        summaryLabel = new QLabel(summary, this);
        summaryLabel->setObjectName("prefRowSummary");
        summaryLabel->setStyleSheet("color: palette(mid); font-size: 12px;");
        summaryLabel->setWordWrap(true);
        summaryLabel->setVisible(!summary.isEmpty());
        textCol->addWidget(summaryLabel);

        outer->addLayout(textCol, /*stretch*/ 1);
        controlSlot = new QHBoxLayout();
        controlSlot->setSpacing(0);
        outer->addLayout(controlSlot);

        setMinimumHeight(48);
    }

    // Places `control` in the trailing slot (a QLineEdit, QComboBox,
    // QCheckBox, QSpinBox, etc.) - ownership stays with the caller's parent
    // chain via Qt's normal child-widget mechanics.
    void setControl(QWidget *control) {
        while (auto *item = controlSlot->takeAt(0)) {
            delete item;
        }
        if (control != nullptr) {
            controlSlot->addWidget(control);
        }
    }

    void setSummary(const QString &summary) {
        summaryLabel->setText(summary);
        summaryLabel->setVisible(!summary.isEmpty());
    }

    QLabel *titleLabel;
    QLabel *summaryLabel;

signals:
    // Emitted on click anywhere in the row outside the trailing control -
    // mirrors Android's tap-anywhere-on-the-row-to-activate Preference
    // behavior (e.g. tapping a switch preference's title toggles it too).
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override {
        QWidget::mouseReleaseEvent(event);
        emit clicked();
    }

private:
    QHBoxLayout *controlSlot;
};
