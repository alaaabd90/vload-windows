#pragma once

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

// Mirrors Android's bottom StatsBar (layout_main.xml lines ~35-99): a
// colorPrimary-tinted strip anchored under the profile list showing
// connection status plus live tx/rx throughput, with the ConnectFab
// overlapping its top edge (matching Android's FAB-anchored-to-bottom-bar
// layout).
class StatsBar : public QWidget {
    Q_OBJECT
public:
    explicit StatsBar(QWidget *parent = nullptr) : QWidget(parent) {
        setObjectName("statsBar");
        setFixedHeight(56);

        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(20, 8, 20, 8);

        statusLabel = new QLabel(tr("Not connected"), this);
        layout->addWidget(statusLabel, /*stretch*/ 1);

        trafficLabel = new QLabel("↓ 0 B/s   ↑ 0 B/s", this);
        layout->addWidget(trafficLabel);
    }

    void setStatus(const QString &text) { statusLabel->setText(text); }

    void setTraffic(const QString &rx, const QString &tx) {
        trafficLabel->setText(QStringLiteral("↓ %1/s   ↑ %2/s").arg(rx, tx));
    }

private:
    QLabel *statusLabel;
    QLabel *trafficLabel;
};
