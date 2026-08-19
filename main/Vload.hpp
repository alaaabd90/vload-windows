#pragma once

#include "Const.hpp"
#include "Vload_Utils.hpp"
#include "Vload_ConfigItem.hpp"
#include "Vload_DataStore.hpp"

// Switch core support

namespace Vload {
    inline int coreType = CoreType::SING_BOX;

    QString FindCoreAsset(const QString &name);

    QString FindVloadCoreRealPath();

    bool IsAdmin();
} // namespace Vload

#define ROUTES_PREFIX_NAME QStringLiteral("routes_box")
#define ROUTES_PREFIX QString(ROUTES_PREFIX_NAME + "/")
