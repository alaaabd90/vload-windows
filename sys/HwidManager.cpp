#include "HwidManager.hpp"

#include <QCryptographicHash>
#include <QSettings>
#include <QSysInfo>

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
