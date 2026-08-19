#pragma once

#include <QByteArray>
#include <QString>

// Encrypts an exported profile config so it only decrypts on the device
// whose HWID (see sys/HwidManager.hpp) it was locked to. The HWID doubles
// as the AES-256 key (SHA-256 of the uppercase hex string), so even
// bypassing the app's own HWID-match check wouldn't be enough to decrypt
// the payload - matches Android's utils/LockedProfileCrypto.kt exactly so
// .vloadp files move between phone and PC.
//
// Binary format (VERSION 2): MAGIC("VLDP",4) | VERSION(1) | HWID(32 ASCII) | IV(12) | CIPHERTEXT
// CIPHERTEXT is AES-256-GCM output with the 16-byte auth tag appended, same
// layout javax.crypto.Cipher.doFinal() produces on the Android side.
//
// VERSION 1 files (16 ASCII HWID field, matching the pre-fix HwidManager's
// shorter, FINGERPRINT-dependent digest on both platforms) are recognized
// and rejected with a clear error rather than misparsed against the new
// fixed field width - see HwidManager.cpp for why that format changed.
namespace Vload_fmt {
    // Public so UI input validation (the "lock for another device" HWID
    // entry field) can check against the same length instead of a
    // hand-copied magic number that could drift out of sync with this.
    constexpr int LockedProfileHwidChars = 32;

    enum class LockedProfileResultType {
        NotLocked,
        WrongDevice,
        Decrypted,
        Error,
    };

    struct LockedProfileDecryptResult {
        LockedProfileResultType type;
        QString plaintext;    // valid when type == Decrypted
        QString lockedToHwid; // valid when type == WrongDevice
        QString error;        // valid when type == Error
    };

    QByteArray LockedProfileCrypto_EncryptForHwid(const QString &plaintext, const QString &recipientHwid);

    LockedProfileDecryptResult LockedProfileCrypto_TryDecrypt(const QByteArray &content, const QString &deviceHwid);
} // namespace Vload_fmt
