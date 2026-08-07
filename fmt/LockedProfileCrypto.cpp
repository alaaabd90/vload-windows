#include "LockedProfileCrypto.hpp"

#include <QCryptographicHash>
#include <QRandomGenerator>

#ifdef Q_OS_WIN

#include <windows.h>
#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

namespace NekoGui_fmt {
    namespace {
        constexpr int GCM_TAG_BYTES = 16;
        constexpr int GCM_IV_BYTES = 12;
        constexpr int HWID_BYTES = 16;
        const QByteArray MAGIC = QByteArrayLiteral("VLDP");
        constexpr char VERSION = 0x01;
        constexpr int HEADER_SIZE = 4 + 1 + HWID_BYTES + GCM_IV_BYTES; // 33

        QByteArray DeriveKey(const QString &hwid) {
            return QCryptographicHash::hash(hwid.toUpper().toUtf8(), QCryptographicHash::Sha256);
        }

        // AES-256-GCM via Windows CNG (bcrypt.dll) - no third-party crypto
        // dependency needed on Windows. encrypt=true: tagOut receives the
        // computed 16-byte tag. encrypt=false: tagIn must hold the tag to
        // verify against (decryption fails if it doesn't match).
        bool AesGcm(bool encrypt, const QByteArray &key, const QByteArray &iv,
                    const QByteArray &input, QByteArray &tag, QByteArray &output) {
            BCRYPT_ALG_HANDLE hAlg = nullptr;
            BCRYPT_KEY_HANDLE hKey = nullptr;
            bool ok = false;

            if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, nullptr, 0) < 0) return false;
            if (BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, (PBYTE) BCRYPT_CHAIN_MODE_GCM,
                                   sizeof(BCRYPT_CHAIN_MODE_GCM), 0) < 0) {
                BCryptCloseAlgorithmProvider(hAlg, 0);
                return false;
            }

            auto keyBytes = key;
            if (BCryptGenerateSymmetricKey(hAlg, &hKey, nullptr, 0, (PUCHAR) keyBytes.data(), (ULONG) keyBytes.size(), 0) < 0) {
                BCryptCloseAlgorithmProvider(hAlg, 0);
                return false;
            }

            BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO authInfo;
            BCRYPT_INIT_AUTH_MODE_INFO(authInfo);
            auto ivBytes = iv;
            authInfo.pbNonce = (PUCHAR) ivBytes.data();
            authInfo.cbNonce = (ULONG) ivBytes.size();

            output.resize(input.size());
            ULONG resultLen = 0;
            NTSTATUS status;

            if (encrypt) {
                QByteArray tagBuf(GCM_TAG_BYTES, 0);
                authInfo.pbTag = (PUCHAR) tagBuf.data();
                authInfo.cbTag = (ULONG) tagBuf.size();
                status = BCryptEncrypt(hKey, (PUCHAR) input.data(), (ULONG) input.size(), &authInfo,
                                        nullptr, 0, (PUCHAR) output.data(), (ULONG) output.size(), &resultLen, 0);
                if (status >= 0) {
                    tag = tagBuf;
                    ok = true;
                }
            } else {
                authInfo.pbTag = (PUCHAR) tag.data();
                authInfo.cbTag = (ULONG) tag.size();
                status = BCryptDecrypt(hKey, (PUCHAR) input.data(), (ULONG) input.size(), &authInfo,
                                        nullptr, 0, (PUCHAR) output.data(), (ULONG) output.size(), &resultLen, 0);
                ok = status >= 0;
            }

            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return ok;
        }
    } // namespace

    QByteArray LockedProfileCrypto_EncryptForHwid(const QString &plaintext, const QString &recipientHwid) {
        auto hwid = recipientHwid.toUpper();
        auto key = DeriveKey(hwid);

        QByteArray iv(GCM_IV_BYTES, 0);
        QRandomGenerator::system()->fillRange(reinterpret_cast<quint32 *>(iv.data()), GCM_IV_BYTES / int(sizeof(quint32)));

        QByteArray tag;
        QByteArray cipherBytes;
        if (!AesGcm(true, key, iv, plaintext.toUtf8(), tag, cipherBytes)) {
            return {};
        }

        QByteArray result;
        result += MAGIC;
        result += VERSION;
        result += hwid.toLatin1();
        result += iv;
        result += cipherBytes;
        result += tag;
        return result;
    }

    LockedProfileDecryptResult LockedProfileCrypto_TryDecrypt(const QByteArray &content, const QString &deviceHwid) {
        if (content.size() < HEADER_SIZE + GCM_TAG_BYTES || !content.startsWith(MAGIC)) {
            return {LockedProfileResultType::NotLocked, {}, {}, {}};
        }
        auto version = content[4];
        if (version != VERSION) {
            return {LockedProfileResultType::Error, {}, {}, QStringLiteral("Unsupported locked-profile format version %1").arg(int(version))};
        }

        auto lockedTo = QString::fromLatin1(content.mid(5, HWID_BYTES)).toUpper();
        if (lockedTo != deviceHwid.toUpper()) {
            return {LockedProfileResultType::WrongDevice, {}, lockedTo, {}};
        }

        auto iv = content.mid(5 + HWID_BYTES, GCM_IV_BYTES);
        auto cipherAndTag = content.mid(HEADER_SIZE);
        auto tag = cipherAndTag.right(GCM_TAG_BYTES);
        auto cipherBytes = cipherAndTag.left(cipherAndTag.size() - GCM_TAG_BYTES);

        auto key = DeriveKey(deviceHwid);
        QByteArray plainBytes;
        if (!AesGcm(false, key, iv, cipherBytes, tag, plainBytes)) {
            return {LockedProfileResultType::Error, {}, {}, QStringLiteral("Decryption failed")};
        }

        return {LockedProfileResultType::Decrypted, QString::fromUtf8(plainBytes), {}, {}};
    }
} // namespace NekoGui_fmt

#else

// Locked-profile export/import is Windows-only for now (Windows CNG/bcrypt
// backs the AES-256-GCM impl above) - not yet ported to Linux/macOS.
// Stubbed here so non-Windows builds still link instead of failing on
// windows.h; callers see NotLocked/empty output rather than a crash.
namespace NekoGui_fmt {
    QByteArray LockedProfileCrypto_EncryptForHwid(const QString &, const QString &) {
        return {};
    }

    LockedProfileDecryptResult LockedProfileCrypto_TryDecrypt(const QByteArray &, const QString &) {
        return {LockedProfileResultType::NotLocked, {}, {}, {}};
    }
} // namespace NekoGui_fmt

#endif
