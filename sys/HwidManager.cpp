#include "HwidManager.hpp"

#include <QCryptographicHash>
#include <QSettings>
#include <QSysInfo>

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

QString HwidManager_Compute() {
    auto machineGuid = GetMachineGuid();
    auto fingerprint = QSysInfo::prettyProductName() + "/" + QSysInfo::kernelVersion() + "/" + QSysInfo::buildAbi();
    auto serial = GetVolumeSerial();

    auto input = machineGuid + "|" + fingerprint + "|" + serial;
    auto digest = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(digest.left(8).toHex()).toUpper();
}

#else

// HWID derivation is Windows-only for now (registry MachineGuid + volume
// serial, both Windows-native concepts) - not yet ported to Linux/macOS.
QString HwidManager_Compute() {
    auto fingerprint = QSysInfo::prettyProductName() + "/" + QSysInfo::kernelVersion() + "/" + QSysInfo::buildAbi();
    auto digest = QCryptographicHash::hash(fingerprint.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(digest.left(8).toHex()).toUpper();
}

#endif
