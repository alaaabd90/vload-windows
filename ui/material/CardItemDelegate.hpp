#pragma once

#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>

// Custom roles carrying the extra per-row data layout_profile.xml's card
// needs beyond Qt::DisplayRole (name) - set via QListWidgetItem::setData.
namespace CardRole {
    constexpr int AccentColor = Qt::UserRole + 1; // QColor - group's assigned MaterialTheme.primary
    constexpr int Address = Qt::UserRole + 2;     // QString
    constexpr int TypeAndStatus = Qt::UserRole + 3; // QString, e.g. "VLESS - Connected"
    constexpr int Selected = Qt::UserRole + 4;    // bool - this profile is the active/started one
    constexpr int LockedImport = Qt::UserRole + 5; // bool - suppress Address/TypeAndStatus (see HWID lock feature)
}

// Paints one row exactly matching Android's layout_profile.xml Material
// CardView: a colored left accent stripe, bold profile name, address line,
// protocol-type + status line, all on a slightly-elevated card background.
class CardItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit CardItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        auto rect = option.rect.adjusted(6, 4, -6, -4);
        bool selected = index.data(CardRole::Selected).toBool();
        bool locked = index.data(CardRole::LockedImport).toBool();
        QColor accent = index.data(CardRole::AccentColor).value<QColor>();
        if (!accent.isValid()) accent = QColor("#9C27B0");

        // Card background
        bool dark = option.palette.window().color().lightness() < 128;
        QColor cardBg = dark ? QColor("#2A2A2A") : QColor("#FFFFFF");
        if (option.state & QStyle::State_MouseOver) {
            cardBg = dark ? cardBg.lighter(115) : cardBg.darker(103);
        }
        painter->setPen(Qt::NoPen);
        painter->setBrush(cardBg);
        painter->drawRoundedRect(rect, 8, 8);

        // Left accent stripe
        auto stripeRect = QRect(rect.left(), rect.top(), 4, rect.height());
        painter->setBrush(accent);
        painter->drawRoundedRect(stripeRect, 2, 2);
        if (selected) {
            // Selected/running profile gets a full accent-tinted left edge glow,
            // mirroring Android's selected-card accent border.
            painter->setPen(QPen(accent, 2));
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(rect.adjusted(1, 1, -1, -1), 8, 8);
        }

        auto textRect = rect.adjusted(18, 6, -12, -6);
        QColor textPrimary = dark ? QColor("#E0E0E0") : QColor("#212121");
        QColor textSecondary = dark ? QColor("#A0A0A0") : QColor("#616161");

        QFont nameFont = option.font;
        nameFont.setBold(true);
        nameFont.setPointSize(nameFont.pointSize() + 1);
        painter->setFont(nameFont);
        painter->setPen(textPrimary);
        auto nameRect = QRect(textRect.left(), textRect.top(), textRect.width(), 20);
        painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter,
                           index.data(Qt::DisplayRole).toString());

        painter->setFont(option.font);
        painter->setPen(textSecondary);
        if (!locked) {
            auto addrRect = QRect(textRect.left(), textRect.top() + 20, textRect.width(), 18);
            painter->drawText(addrRect, Qt::AlignLeft | Qt::AlignVCenter,
                               index.data(CardRole::Address).toString());
            auto statusRect = QRect(textRect.left(), textRect.top() + 38, textRect.width(), 18);
            painter->drawText(statusRect, Qt::AlignLeft | Qt::AlignVCenter,
                               index.data(CardRole::TypeAndStatus).toString());
        } else {
            // Locked-import profiles hide protocol/address, matching
            // ConfigurationFragment.kt's lockedImport UI suppression.
            auto lockRect = QRect(textRect.left(), textRect.top() + 20, textRect.width(), 18);
            painter->drawText(lockRect, Qt::AlignLeft | Qt::AlignVCenter, tr("Locked profile"));
        }

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        Q_UNUSED(option)
        Q_UNUSED(index)
        return QSize(200, 78);
    }
};
