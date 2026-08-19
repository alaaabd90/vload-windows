#include <QApplication>

#include "ThemeManager.hpp"
#include "main/MaterialPalette.hpp"

ThemeManager *themeManager = new ThemeManager;

extern QString ReadFileText(const QString &path);

void ThemeManager::ApplyTheme(const QString &themeName, bool dark) {
    if (current_theme == themeName && current_dark == dark) return;

    const auto &t = MaterialPalette::byName(themeName.isEmpty() ? "Pink_SSR" : themeName);

    QString windowBg, surfaceBg, textPrimary, textSecondary, divider;
    if (dark) {
        windowBg = "#121212";
        surfaceBg = "#1E1E1E";
        textPrimary = "#E0E0E0";
        textSecondary = "#A0A0A0";
        divider = "#333333";
    } else {
        windowBg = "#FAFAFA";
        surfaceBg = "#FFFFFF";
        textPrimary = "#212121";
        textSecondary = "#616161";
        divider = "#E0E0E0";
    }
    QString textOnAccent = t.darkText ? "#000000" : "#FFFFFF";

    QString qss = ReadFileText(":/theme/material/base.qss.tpl");
    qss.replace("%PRIMARY%", t.primary);
    qss.replace("%PRIMARY_DARK%", t.primaryDark);
    qss.replace("%ACCENT%", t.accent);
    qss.replace("%MATERIAL_100%", t.material100);
    qss.replace("%MATERIAL_300%", t.material300);
    qss.replace("%WINDOW_BG%", windowBg);
    qss.replace("%SURFACE_BG%", surfaceBg);
    qss.replace("%TEXT_PRIMARY%", textPrimary);
    qss.replace("%TEXT_SECONDARY%", textSecondary);
    qss.replace("%DIVIDER%", divider);
    qss.replace("%TEXT_ON_ACCENT%", textOnAccent);

    auto vload_css = ReadFileText(":/vload/vload.css");
    qApp->setStyleSheet(qss + "\n" + vload_css);

    current_theme = themeName;
    current_dark = dark;
}
