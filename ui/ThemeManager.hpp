#pragma once

class ThemeManager {
public:
    // Theme *name* (e.g. "Pink_SSR", "Blue" - see main/MaterialPalette.hpp),
    // not the old feiyangqingyun int id. Orthogonal to dark mode.
    QString current_theme = "";
    bool current_dark = false;

    void ApplyTheme(const QString &themeName, bool dark);
};

extern ThemeManager *themeManager;
