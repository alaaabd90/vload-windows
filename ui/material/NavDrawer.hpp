#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QFrame>

// Mirrors Android's hamburger nav drawer (main_drawer_menu.xml): a slide-out
// overlay panel over the main content, listing Configuration/Group/Route/
// Settings, then Logcat/Traffic/Tools, then About. (Android's "Ads"/"FAQ"
// entries are intentionally not ported - the equivalent toolbar buttons were
// already removed from the Windows app per explicit direction earlier this
// project.) Qt Widgets has no built-in drawer widget, so this is a plain
// QWidget positioned outside the normal layout (raised above the main
// content, animated via QPropertyAnimation on its x position) rather than a
// dedicated Qt drawer class, which doesn't exist.
class NavDrawer : public QWidget {
    Q_OBJECT
public:
    explicit NavDrawer(QWidget *parent) : QWidget(parent) {
        setObjectName("navDrawer");
        setFixedWidth(280);
        raise();

        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 16, 0, 16);
        layout->setSpacing(0);

        addItem(layout, tr("Configuration"), "nav_configuration");
        addItem(layout, tr("Group"), "nav_group");
        addItem(layout, tr("Route"), "nav_route");
        addItem(layout, tr("Settings"), "nav_settings");
        addDivider(layout);
        addItem(layout, tr("Log"), "nav_logcat");
        addItem(layout, tr("Traffic"), "nav_traffic");
        addItem(layout, tr("Tools"), "nav_tools");
        addDivider(layout);
        addItem(layout, tr("About"), "nav_about");
        layout->addStretch(1);

        anim = new QPropertyAnimation(this, "pos", this);
        anim->setDuration(220);
        anim->setEasingCurve(QEasingCurve::OutCubic);

        hide();
    }

    void toggle() { setOpen(!open_); }

    void setOpen(bool open) {
        open_ = open;
        setFixedHeight(parentWidget()->height());
        int closedX = -width();
        int openX = 0;
        if (open) show();
        anim->stop();
        anim->setStartValue(pos());
        anim->setEndValue(QPoint(open ? openX : closedX, 0));
        if (!open) {
            connect(anim, &QPropertyAnimation::finished, this, [=] {
                if (!open_) hide();
            });
        }
        anim->start();
        raise();
    }

signals:
    void itemActivated(const QString &itemId);

private:
    bool open_ = false;
    QPropertyAnimation *anim;

    void addItem(QVBoxLayout *layout, const QString &title, const QString &id) {
        auto *btn = new QPushButton(title, this);
        btn->setObjectName("navDrawerItem");
        btn->setFlat(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet("text-align: left; padding: 12px 24px; border: none; font-size: 13px;");
        connect(btn, &QPushButton::clicked, this, [=] {
            emit itemActivated(id);
            setOpen(false);
        });
        layout->addWidget(btn);
    }

    void addDivider(QVBoxLayout *layout) {
        auto *line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet("color: palette(mid); margin: 8px 16px;");
        layout->addWidget(line);
    }
};
