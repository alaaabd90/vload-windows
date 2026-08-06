#pragma once

#include <QWidget>
#include <QPainter>
#include <QPropertyAnimation>
#include <QMouseEvent>

// Mirrors Android's circular FAB connect button (ServiceButton, see
// layout_main.xml) - a custom-painted circular button with an animated
// progress ring shown while connecting, and a status-dependent fill color
// (idle/connecting/connected/error), replacing the old VPN/SystemProxy
// checkboxes.
class ConnectFab : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int ringAngle READ ringAngle WRITE setRingAngle)

public:
    enum class State { Idle, Connecting, Connected, SystemProxy, Error };

    explicit ConnectFab(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedSize(64, 64);
        setCursor(Qt::PointingHandCursor);

        ringAnim = new QPropertyAnimation(this, "ringAngle", this);
        ringAnim->setStartValue(0);
        ringAnim->setEndValue(360 * 16); // QPainter angles are in 1/16th degrees
        ringAnim->setDuration(1200);
        ringAnim->setLoopCount(-1);
    }

    void setState(State s) {
        if (state_ == s) return;
        state_ = s;
        if (s == State::Connecting) {
            ringAnim->start();
        } else {
            ringAnim->stop();
            ringAngle_ = 0;
        }
        update();
    }

    State state() const { return state_; }

    int ringAngle() const { return ringAngle_; }
    void setRingAngle(int a) {
        ringAngle_ = a % (360 * 16);
        update();
    }

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override {
        QWidget::mouseReleaseEvent(event);
        if (rect().contains(event->pos())) emit clicked();
    }

    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        QColor fill;
        switch (state_) {
            case State::Idle: fill = QColor("#9E9E9E"); break;
            case State::Connecting: fill = QColor("#FFC107"); break;
            case State::Connected: fill = QColor("#4CAF50"); break;
            case State::SystemProxy: fill = QColor("#2196F3"); break;
            case State::Error: fill = QColor("#F44336"); break;
        }

        auto r = rect().adjusted(6, 6, -6, -6);
        p.setPen(Qt::NoPen);
        p.setBrush(fill);
        p.drawEllipse(r);

        if (state_ == State::Connecting) {
            QPen ringPen(QColor(255, 255, 255, 200), 3);
            p.setPen(ringPen);
            p.setBrush(Qt::NoBrush);
            auto ringRect = rect().adjusted(2, 2, -2, -2);
            p.drawArc(ringRect, ringAngle_, 90 * 16);
        }

        // Simple play/stop glyph, matching Android's idle/connected icon swap
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::white);
        auto c = r.center();
        if (state_ == State::Idle) {
            QPolygon triangle;
            triangle << QPoint(c.x() - 6, c.y() - 9) << QPoint(c.x() - 6, c.y() + 9) << QPoint(c.x() + 9, c.y());
            p.drawPolygon(triangle);
        } else {
            p.drawRoundedRect(QRect(c.x() - 7, c.y() - 7, 14, 14), 2, 2);
        }
    }

private:
    State state_ = State::Idle;
    int ringAngle_ = 0;
    QPropertyAnimation *ringAnim;
};
