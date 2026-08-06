#pragma once

#include <QString>

// Deterministic per-machine fingerprint, recomputed from machine properties
// rather than a randomly generated and persisted value - stays stable across
// reinstalls on the same physical machine. Mirrors Android's HwidManager.kt
// (SHA256(ANDROID_ID|Build.FINGERPRINT|Build.SERIAL), first 8 bytes as 16
// uppercase hex chars) with Windows-native equivalents for each input:
//   - Machine GUID (HKLM\SOFTWARE\Microsoft\Cryptography\MachineGuid) in
//     place of ANDROID_ID - a persistent per-install pseudo-random ID.
//   - OS version/build string in place of Build.FINGERPRINT.
//   - The system volume's serial number in place of Build.SERIAL.
// Not tamper-proof (all of these are readable/spoofable by an admin), just a
// casual deterrent against using a profile on the wrong machine - same as
// the Android side already documents itself as being.
QString HwidManager_Compute();
