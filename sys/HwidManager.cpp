#include "HwidManager.hpp"

#include <QCryptographicHash>
#include <QSettings>
#include <QSysInfo>

// 16 bytes -> 32 hex chars, matching LockedProfileCrypto's HWID_BYTES on
// both this platform and Android (utils/HwidManager.kt) - keep in sync.
constexpr int HWID_LENGTH_BYTES = 16;

#ifdef Q_OS_WIN

#include <windows.h>

namespace {
    QString GetMachineGuid() {
        QSettings reg(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography)", QSettings::NativeFormat);
        return reg.value("MachineGuid").toString();
    }

    QString GetVolumeSerial() {
        DWORD serial = 0;
        if (GetVolumeInformationW(L"C:\\", nullptr, 0, &serial, nullptr, nullptr, nullptr, 0)) {
            return QString::number(serial, 16).toUpper();
        }
        return "unknown";
    }
} // namespace

// Deliberately excludes QSysInfo::prettyProductName()/kernelVersion(),
// which the previous version of this mixed in: both change on routine
// Windows Update feature/cumulative updates, not just reinstalls - directly
// broke "same HWID for this PC always" the moment Windows auto-updated
// (the exact same class of bug Android's Build.FINGERPRINT caused there;
// see utils/HwidManager.kt on the Android side for the full writeup).
// MachineGuid is generated once at Windows install and survives this app
// being uninstalled/reinstalled; buildAbi() is a static CPU/ABI descriptor
// (stable across updates), kept for a little extra binding without being
// able to change the fingerprint by itself. The volume serial is genuinely
// stable short of reformatting C:, which is a legitimately-different-machine
// event, not routine drift.
QString HwidManager_Compute() {
    auto machineGuid = GetMachineGuid();
    auto abi = QSysInfo::buildAbi();
    auto serial = GetVolumeSerial();

    auto input = machineGuid + "|" + abi + "|" + serial;
    auto digest = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(digest.left(HWID_LENGTH_BYTES).toHex()).toUpper();
}

#else

// HWID derivation's stable-primary-identifier path (registry MachineGuid +
// volume serial) is Windows-only for now - not yet ported to Linux/macOS.
// QSysInfo::machineUniqueId() is Qt's own cross-platform stable machine ID
// (backed by /etc/machine-id on Linux, IOPlatformUUID on macOS) - a real
// per-device identifier rather than nothing, though still less specific
// than the Windows path above pending an actual per-platform port.
QString HwidManager_Compute() {
    auto machineId = QString::fromUtf8(QSysInfo::machineUniqueId());
    auto abi = QSysInfo::buildAbi();
    auto input = machineId + "|" + abi;
    auto digest = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(digest.left(HWID_LENGTH_BYTES).toHex()).toUpper();
}

#endif
