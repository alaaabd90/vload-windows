#pragma once

#include <QListWidget>
#include <QDragMoveEvent>

#include "CardItemDelegate.hpp"

// Replaces MyTableWidget/proxyListTable's row-based table with Android's
// Material CardView list look (layout_profile.xml), while preserving the
// same drag-to-reorder + id-tracking contract mainwindow.cpp already
// depends on (order/id2Row/row2Id, callback_save_order) so the rest of the
// app's profile-list logic doesn't need to change shape, just which widget
// it's hosted in.
class CardListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit CardListWidget(QWidget *parent = nullptr) : QListWidget(parent) {
        setItemDelegate(new CardItemDelegate(this));
        setDragDropMode(QAbstractItemView::InternalMove);
        setDropIndicatorShown(true);
        setSelectionMode(QAbstractItemView::SingleSelection);
        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        setSpacing(2);
        setFrameShape(QFrame::NoFrame);
        setUniformItemSizes(true);
    }

    QList<int> order;          // profile id, in display order (persisted)
    std::map<int, int> id2Row;
    QList<int> row2Id;

    std::function<void()> callback_save_order;
    std::function<void(int id)> refresh_data;

    // Adds/updates one profile row. id is the ProxyEntity id (matches the
    // existing table's row2Id/id2Row contract). Pass -1 accentRgb to fall
    // back to the group's rotation-assigned MaterialPalette color.
    void setProfileRow(int row, int id, const QString &name, const QString &address,
                       const QString &typeAndStatus, const QColor &accent,
                       bool selected, bool lockedImport) {
        QListWidgetItem *item;
        if (row < count()) {
            item = this->item(row);
        } else {
            item = new QListWidgetItem();
            insertItem(row, item);
        }
        item->setData(Qt::DisplayRole, name);
        item->setData(CardRole::Address, address);
        item->setData(CardRole::TypeAndStatus, typeAndStatus);
        item->setData(CardRole::AccentColor, accent);
        item->setData(CardRole::Selected, selected);
        item->setData(CardRole::LockedImport, lockedImport);
        item->setData(Qt::UserRole, id); // profile id, read back by row2Id rebuild below

        while (row2Id.size() <= row) row2Id.append(-1);
        row2Id[row] = id;
        id2Row[id] = row;
    }

    void clearRows() {
        clear();
        row2Id.clear();
        id2Row.clear();
    }

    void _save_order(bool saveToFile) {
        order.clear();
        id2Row.clear();
        for (int i = 0; i < count(); i++) {
            auto id = item(i)->data(Qt::UserRole).toInt();
            order += id;
            id2Row[id] = i;
            while (row2Id.size() <= i) row2Id.append(-1);
            row2Id[i] = id;
        }
        if (callback_save_order != nullptr && saveToFile) callback_save_order();
    }

protected:
    void dropEvent(QDropEvent *event) override {
        QListWidget::dropEvent(event);
        _save_order(true);
    }
};
