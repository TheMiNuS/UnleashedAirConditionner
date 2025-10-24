# Firmware Signing — How-To (ESP8266/ESP12)

This folder contains the **script to generate signing keys** and the usage notes to **sign your firmware** so the device only accepts trusted OTA updates.

> ⚠️ **Do not commit private keys.**

---

## Files

- `Generate_Firmware_Signing_Certificates.sh` — helper script to create:
  - `public.key`  (safe to publish)
  - `private.key` (**DO NOT PUBLISH**)
- (optional) generated signatures next to your `.bin` when signing (see below)

If you already have keys, place them here as:
```
signing/
├── public.key           # publishable
└── private.key          # keep secret, gitignored
```

---

## Generate keys

From this folder:
```bash
bash ./Generate_Firmware_Signing_Certificates.sh
```
It will create `public.key` and `private.key` in the current directory.
- **Never commit** `private.key` `public.key` (the `.gitignore` at repo root already ignores it).

---

## Sign a firmware binary

Use the provided signing tool (example):
```bash
# From the repository root after you built your firmware with PlatformIO
# Example output path; adjust to your env:
BIN=".pio/build/esp12e/firmware.bin"

# Produce firmware.bin.sig using private.key
python3 ./sign_firmware.py --key signing/private.key --in "$BIN" --out "$BIN.sig"
```

You can then publish `firmware.bin` and `firmware.bin.sig` together on your update server or distribute them both during OTA.

> The device embeds `public.key` and verifies `firmware.bin` with `firmware.bin.sig` before applying the update.

---

## Firmware build notes

- Ensure the **public key** is compiled into the firmware (e.g., as a PEM literal).
- The bootloader / update code must **verify the signature** (SHA-256 + RSA) *before* flashing.
- In the app code, **reject OTA** if the signature is missing/invalid.

---

## Security checklist

- [ ] `private.key` is stored outside of Git and backed up securely.
- [ ] CI secrets (if used) are scoped to signing only.
- [ ] OTA endpoints require authentication (or are not exposed on the open internet).
- [ ] Device refuses unsigned or mismatched binaries.
- [ ] You rotate keys if a leak is suspected.

---

## Licensing

This documentation is provided as-is. You are responsible for safeguarding your private keys and complying with local regulations and dependencies’ licenses.
