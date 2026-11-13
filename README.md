# Moto G7 Play mainline Linux

Mainline Linux port for the **Motorola Moto G7 Play** (`channel`, XT1952-x, Qualcomm SDM632).
Runs [postmarketOS](https://postmarketos.org) with [sxmo](https://sxmo.org) on top of the
[msm8953-mainline](https://github.com/msm8953-mainline/linux) kernel (tested on `v7.1.3-r0`).

Before this there was no device tree, no lk2nd support and no pmaports package for `channel`.
The DTS is based on `sdm632-motorola-ocean.dts` (Moto G7 Power).

## Status

| Feature | Status | Notes |
|---|---|---|
| Boot | ✅ | lk2nd (generic msm8953 build) → extlinux |
| Display | ✅ | DJN 569 panel (`motorola,channel-569-djn`), own driver. The DJN 568, DSBJ 569 and DSBJ 570 variants have no driver yet |
| Backlight | ✅ | DCS |
| Touch | ✅ | NT36525 via `novatek-nvt-ts` |
| Wi-Fi | ✅ | WCN3620, 2.4 GHz only |
| USB (device), SSH | ✅ | Type-C via PMI632 |
| Charging | ✅ | Done by PMI632 hardware, no driver |
| Battery % | ⚠️ | Userspace estimate from PMI632 ADC voltage (±10%) |
| Vibration | ✅ | TI DRV2624, own driver |
| Buttons | ✅ | Power, volume up/down |
| GPU | ❓ | Untested (zap shader from `ocean` loads) |
| Audio, modem, camera, sensors | ❌ | Not started |
| Suspend | ❌ | Kept awake with a wakelock |

## Contents

```
dts/sdm632-motorola-channel.dts            device tree (goes in arch/arm64/boot/dts/qcom/)
drivers/panel-motorola-channel-569-djn/    DSI panel driver (out of tree)
drivers/drv2624/                           TI DRV2624 haptics driver (out of tree)
sxmo/sxmo_deviceprofile_motorola,channel.sh  sxmo device profile
sxmo/adc-battery.sh                        battery % for the sxmo status bar
sxmo/sxmo_battery_monitor.service          superd override that runs it
```

No firmware is included. The Wi-Fi NV file (`WCNSS_qcom_wlan_nv.bin`) comes from the
stock `vendor` partition and goes in `/lib/firmware/qcom/msm8953/motorola/channel/`.

## Building

In a msm8953-mainline kernel tree prepared with the running kernel's config
(`make olddefconfig modules_prepare`):

```sh
# DTB
cp dts/sdm632-motorola-channel.dts $KERNEL/arch/arm64/boot/dts/qcom/
cpp -nostdinc -undef -D__DTS__ -x assembler-with-cpp \
    -I $KERNEL/include -I $KERNEL/arch/arm64/boot/dts \
    -I $KERNEL/arch/arm64/boot/dts/qcom -I $KERNEL/scripts/dtc/include-prefixes \
    $KERNEL/arch/arm64/boot/dts/qcom/sdm632-motorola-channel.dts |
    dtc -@ -I dts -O dtb -o sdm632-motorola-channel.dtb -

# Modules
make -C $KERNEL ARCH=arm64 LLVM=1 KBUILD_MODPOST_WARN=1 M=$PWD/drivers/panel-motorola-channel-569-djn modules
make -C $KERNEL ARCH=arm64 LLVM=1 KBUILD_MODPOST_WARN=1 M=$PWD/drivers/drv2624 modules
```

## Installing

Rough outline until the kernel and pmaports changes are merged (then the stock `qcom-msm8953` image will just work).

The DTS says `motorola,channel-panel`; lk2nd detects the real panel and puts its compatible in front.

1. Unlock the bootloader (Motorola's official unlock).
2. Flash [lk2nd](https://github.com/msm8916-mainline/lk2nd) (msm8953 build from February 2026 or newer, which knows `channel` and its panels) to **both** `boot_a` and `boot_b`
   from the Motorola bootloader (MBM): `fastboot flash boot_a lk2nd.img`, same for `boot_b`, then `fastboot set_active a`.
3. Take a postmarketOS `qcom-msm8953` image (sxmo), add `sdm632-motorola-channel.dtb` to the
   boot partition and point `extlinux.conf` at it (`fdt /sdm632-motorola-channel.dtb`).
4. From lk2nd's fastboot: `fastboot flash userdata pmos.img`.
5. Install the modules in `/lib/modules/$(uname -r)/extra/`, run `depmod`,
   copy the sxmo files (see their headers), and put `a506_zap.{mdt,b02}` (same as the Moto G7 Power's)
   and the Wi-Fi NV file in `/lib/firmware/qcom/msm8953/motorola/channel/`.

Make sure something marks the slot as successful on every boot (pmOS's `qbootctl.service` does),
or MBM burns the retry counter and stops booting the slot.

### Warnings

- **Never run `fastboot getvar all` in lk2nd** on this phone. It hung lk2nd's fastboot, and
  rebooting from there left no active slot.
- MBM refuses `fastboot boot` and `fastboot flash userdata`. To reach lk2nd's fastboot without
  keys: in MBM run `fastboot erase userdata`, then reboot.
- Booting an Android `boot.img` through lk2nd hangs early (board-id mismatch). Use extlinux.

### Recovering "Failed to initialize partition table"

If MBM shows `Startup failed / Failed to initialize partition table`, the GPT is most likely
fine but **no slot is marked active**. Recovery without blankflash:

1. `fastboot oem blankflash` puts the phone in EDL (`05c6:9008`).
2. Use [bkerler/edl](https://github.com/bkerler/edl) with the matching Motorola loader
   (`lenovo_motorola/000ba0e102e80000_4aac65a9be0cf870_fhprg.bin`, HWID `0x000ba0e102e80000`).
   A wrong loader hangs the PBL; power-cycle and retry.
3. `setactiveslot` returns "command restricted" with that loader, so dump the primary and backup GPT
   (`edl r`/`rs`), set the active + successful bits in the `boot_b` (or `boot_a`) attribute flags,
   recompute the entry-array and header CRC32s, and write them back with `edl ws`
   (primary at sector 0, backup at the last sector).
4. Back up the original GPT first.

## Known issues / TODO

- Panel `vsp`/`vsn` come from the PMI632 LCDB, which has no mainline driver; they stay on from the bootloader.
- DRV2624 voltages are hardcoded for this phone; no auto-calibration.
- Touch uses the `novatek,nt36672a-ts` compatible though the chip is an NT36525.
- `pclk0_clk_src: rcg didn't update` warning at boot (harmless so far).
- No PMI632 fuel gauge / charger driver in mainline yet (WIP QGauge/SMB5 drivers exist in msm8953-mainline PR #239).
- Upstreaming: DTS → linux-arm-msm, panel → dri-devel, DRV2624 → linux-input, lk2nd and pmaports.

## Credits

- `sdm632-motorola-ocean.dts` by Gabriela David, base of the DTS.
- msm8953-mainline, lk2nd and postmarketOS projects.
- [linux-mdss-dsi-panel-driver-generator](https://github.com/msm8916-mainline/linux-mdss-dsi-panel-driver-generator) for the panel driver.
- Motorola's downstream `drv2624_mmi` driver (LineageOS kernel) as register reference.

## License

Per-file SPDX headers: DTS is BSD-3-Clause, kernel drivers GPL-2.0-only,
sxmo device profile AGPL-3.0-only (like sxmo's own profiles), battery script GPL-2.0-only.
