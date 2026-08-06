/* Material theme template - placeholders filled by ThemeManager::ApplyTheme.
   %PRIMARY% %PRIMARY_DARK% %ACCENT% %MATERIAL_100% %MATERIAL_300% come from
   the chosen MaterialTheme (main/MaterialPalette.hpp, mirrors Android's
   Theme.SagerNet.* variants); %WINDOW_BG% %SURFACE_BG% %TEXT_PRIMARY%
   %TEXT_SECONDARY% %DIVIDER% %TEXT_ON_ACCENT% come from light/dark mode. */

QWidget {
    background-color: %WINDOW_BG%;
    color: %TEXT_PRIMARY%;
    selection-background-color: %ACCENT%;
    selection-color: %TEXT_ON_ACCENT%;
}

QMainWindow, QDialog {
    background-color: %WINDOW_BG%;
}

/* Toolbar / header strip - Android's colorPrimary app bar */
#headerBar, #statsBar {
    background-color: %PRIMARY%;
}
#headerBar QLabel, #statsBar QLabel {
    color: %TEXT_ON_ACCENT%;
}

QPushButton, QToolButton {
    background-color: %SURFACE_BG%;
    color: %TEXT_PRIMARY%;
    border: none;
    border-radius: 6px;
    padding: 6px 10px;
}
QPushButton:hover, QToolButton:hover {
    background-color: %MATERIAL_100%;
}
QPushButton:pressed, QToolButton:pressed {
    background-color: %MATERIAL_300%;
}

/* Accent / primary action buttons */
QPushButton[accent="true"] {
    background-color: %ACCENT%;
    color: %TEXT_ON_ACCENT%;
}
QPushButton[accent="true"]:hover {
    background-color: %PRIMARY%;
}

QLineEdit, QPlainTextEdit, QTextEdit, QComboBox, QSpinBox {
    background-color: %SURFACE_BG%;
    color: %TEXT_PRIMARY%;
    border: 1px solid %DIVIDER%;
    border-radius: 4px;
    padding: 4px 6px;
}
QLineEdit:focus, QPlainTextEdit:focus, QComboBox:focus {
    border: 1px solid %ACCENT%;
}

QCheckBox::indicator:checked, QRadioButton::indicator:checked {
    background-color: %ACCENT%;
    border: 1px solid %ACCENT%;
}

QScrollBar:vertical, QScrollBar:horizontal {
    background: transparent;
    width: 10px;
    height: 10px;
    margin: 0px;
}
QScrollBar::handle {
    background: %MATERIAL_300%;
    border-radius: 5px;
    min-height: 20px;
}
QScrollBar::handle:hover {
    background: %ACCENT%;
}
QScrollBar::add-line, QScrollBar::sub-line {
    height: 0px;
    width: 0px;
}

QMenu {
    background-color: %SURFACE_BG%;
    color: %TEXT_PRIMARY%;
    border: 1px solid %DIVIDER%;
}
QMenu::item:selected {
    background-color: %MATERIAL_100%;
}

QTabWidget::pane {
    border: none;
}
QTabBar::tab {
    background: transparent;
    color: %TEXT_SECONDARY%;
    padding: 8px 14px;
}
QTabBar::tab:selected {
    color: %ACCENT%;
    border-bottom: 2px solid %ACCENT%;
}

/* CardListWidget rows (see ui/material/CardItemDelegate) paint their own
   background/accent-stripe directly, but list chrome still follows theme */
QListView, QTreeView, QTableView {
    background-color: %WINDOW_BG%;
    color: %TEXT_PRIMARY%;
    border: none;
    outline: none;
}
QListView::item:selected, QTreeView::item:selected, QTableView::item:selected {
    background-color: %MATERIAL_100%;
    color: %TEXT_PRIMARY%;
}
