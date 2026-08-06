#pragma once

#include <QString>
#include <QList>

// Mirrors the Android app's Theme.SagerNet.* style variants exactly
// (app/src/main/res/values/themes.xml + colors.xml) - same 5 colors per
// theme (colorPrimary, colorPrimaryDark, colorAccent, colorMaterial100,
// colorMaterial300), same names, same hex values, same order as Android's
// material_colors rotation array so a given group index picks the same
// visual theme on both platforms.
struct MaterialTheme {
    QString name;
    QString primary;
    QString primaryDark;
    QString accent;
    QString material100;
    QString material300;
    bool darkText = false; // Android's colorOnPrimarySurface override (Yellow only)
};

namespace MaterialPalette {
    // Order matches Android's res/values/colors.xml <integer-array name="material_colors">
    inline const QList<MaterialTheme> themes = {
        {"Red", "#F44336", "#D32F2F", "#FF5252", "#FFCDD2", "#E57373"},
        {"Pink_SSR", "#fb7299", "#fb7299", "#FF4081", "#FF80AB", "#fb7299"},
        {"Pink", "#E91E63", "#C2185B", "#FF4081", "#F8BBD0", "#F06292"},
        {"Purple", "#9C27B0", "#7B1FA2", "#E040FB", "#E1BEE7", "#BA68C8"},
        {"DeepPurple", "#673AB7", "#512DA8", "#7C4DFF", "#D1C4E9", "#9575CD"},
        {"Indigo", "#3F51B5", "#303F9F", "#536DFE", "#C5CAE9", "#7986CB"},
        {"Blue", "#2196F3", "#1976D2", "#448AFF", "#BBDEFB", "#64B5F6"},
        {"LightBlue", "#03A9F4", "#0288D1", "#40C4FF", "#B3E5FC", "#4FC3F7"},
        {"Cyan", "#00BCD4", "#0097A7", "#18FFFF", "#B2EBF2", "#4DD0E1"},
        {"Teal", "#009688", "#00796B", "#64FFDA", "#B2DFDB", "#4DB6AC"},
        {"Green", "#4CAF50", "#388E3C", "#69F0AE", "#C8E6C9", "#81C784"},
        {"LightGreen", "#8BC34A", "#689F38", "#B2FF59", "#DCEDC8", "#AED581"},
        {"Lime", "#CDDC39", "#AFB42B", "#EEFF41", "#F0F4C3", "#DCE775"},
        {"Yellow", "#FFEB3B", "#FBC02D", "#FFFF00", "#FFF9C4", "#FFF176", true},
        {"Amber", "#FFC107", "#FFA000", "#FFD740", "#FFECB3", "#FFD54F"},
        {"Orange", "#FF9800", "#F57C00", "#FFAB40", "#FFE0B2", "#FFB74D"},
        {"DeepOrange", "#FF5722", "#E64A19", "#FF6E40", "#FFCCBC", "#FF8A65"},
        {"Brown", "#795548", "#5D4037", "#BCAAA4", "#D7CCC8", "#A1887F"},
        {"Grey", "#9E9E9E", "#616161", "#EEEEEE", "#E0E0E0", "#BDBDBD"},
        {"BlueGrey", "#607D8B", "#455A64", "#B0BEC5", "#CFD8DC", "#90A4AE"},
        {"Black", "#2B2B2B", "#2B2B2B", "#9E9E9E", "#2B2B2B", "#2B2B2B"},
    };

    inline const MaterialTheme &byIndex(int index) {
        if (themes.isEmpty()) {
            static MaterialTheme fallback;
            return fallback;
        }
        return themes[((index % themes.size()) + themes.size()) % themes.size()];
    }

    inline const MaterialTheme &byName(const QString &name) {
        for (const auto &t: themes) {
            if (t.name == name) return t;
        }
        return themes.first();
    }
}
