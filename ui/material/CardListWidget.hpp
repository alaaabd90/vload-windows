#pragma once

#include <QListWidget>
#include <QDragMoveEvent>
#include <QMap>

#include "CardItemDelegate.hpp"

// Replaces MyTableWidget/proxyListTable's row-based table with Android's
// Material CardView list look (layout_profile.xml), while preserving the
// exact same two-phase insert/reorder/populate contract mainwindow.cpp
// already depends on:
//   1. insertEmptyRow() for each profile, in natural iteration order
//      (row2Id records that order as rows get inserted)
//   2. update_order() reconciles row2Id against the previously-saved
//      `order` list (by index only - it does NOT touch widget items yet)
//   3. a later refresh_data pass populates each row's actual content by
//      looking up row2Id[row], so sorting/reordering never requires
//      physically moving QListWidgetItems around, just remapping which
//      profile id backs which row index.
class CardListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit CardListWidget(QWidget *parent = nullptr) : QListWidget(parent) {
        setItemDelegate(new CardItemDelegate(this));
        setDragDropMode(QAbstractItemView::InternalMove);
        setDropIndicatorShown(true);
        setSelectionMode(QAbstractItemView::ExtendedSelection);
        setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        setSpacing(2);
        setFrameShape(QFrame::NoFrame);
        setUniformItemSizes(true);
    }

    QList<int> order;          // profile id, in desired display order (persisted)
    std::map<int, int> id2Row; // profile id -> current row index
    QList<int> row2Id;         // row index -> profile id (source of truth for what to render)

    std::function<void()> callback_save_order;
    std::function<void(int id)> refresh_data;

    void insertEmptyRow(int row) {
        auto *item = new QListWidgetItem();
        insertItem(row, item);
    }

    void _save_order(bool saveToFile) {
        order.clear();
        id2Row.clear();
        for (int i = 0; i < count(); i++) {
            auto id = row2Id[i];
            order += id;
            id2Row[id] = i;
        }
        if (callback_save_order != nullptr && saveToFile) callback_save_order();
    }

    // Exact port of MyTableWidget::update_order - reconciles row2Id against
    // the previously-saved `order`, correcting for profiles that were
    // deleted/added since, without touching widget items (those get filled
    // in by refresh_data afterwards, keyed off the reconciled row2Id).
    void update_order(bool saveToFile) {
        if (order.isEmpty()) {
            _save_order(false);
            return;
        }

        bool needSave = false;
        auto deleted_profiles = order;
        for (int i = 0; i < count(); i++) {
            auto id = row2Id[i];
            deleted_profiles.removeAll(id);
        }
        for (auto deleted_profile: deleted_profiles) {
            needSave = true;
            order.removeAll(deleted_profile);
        }

        QMap<int, int> newRows;
        for (int i = 0; i < count(); i++) {
            auto id = row2Id[i];
            auto dst = order.indexOf(id);
            if (dst == i) continue;
            if (dst == -1) {
                needSave = true;
                continue;
            }
            newRows[dst] = id;
        }

        for (int i = 0; i < count(); i++) {
            if (!newRows.contains(i)) continue;
            row2Id[i] = newRows[i];
        }

        _save_order(needSave || saveToFile);
    }

    // Populates row `row`'s visible content - called by refresh_data for
    // each row after row2Id has been finalized for this refresh pass.
    void setRowContent(int row, int id, const QString &name, const QString &address,
                       const QString &typeAndStatus, const QColor &accent,
                       bool selected, bool lockedImport) {
        auto *item = this->item(row);
        if (item == nullptr) return;
        item->setData(Qt::DisplayRole, name);
        item->setData(CardRole::Address, address);
        item->setData(CardRole::TypeAndStatus, typeAndStatus);
        item->setData(CardRole::AccentColor, accent);
        item->setData(CardRole::Selected, selected);
        item->setData(CardRole::LockedImport, lockedImport);
        item->setData(Qt::UserRole, id);
    }

protected:
    void dropEvent(QDropEvent *event) override {
        if (order.isEmpty()) order = row2Id;

        int row_src = currentRow();
        if (row_src < 0 || row_src >= row2Id.size()) return;
        auto id_src = row2Id[row_src];
        // QDropEvent::position() is Qt6-only (QSinglePointEvent); Qt5 (the
        // Linux job builds against 5.12) only has pos(). Both give the
        // same widget-local point for a drop event.
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        auto *dropItem = itemAt(event->position().toPoint());
#else
        auto *dropItem = itemAt(event->pos());
#endif
        if (dropItem == nullptr) return; // dropped below the last row - no-op, matches MyTableWidget
        auto row_dst = this->row(dropItem);

        order.removeAll(id_src);
        order.insert(row_dst, id_src);

        clearSelection();
        update_order(true);
        if (refresh_data != nullptr) refresh_data(-1);
    }
};
