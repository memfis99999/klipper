# 🛠️ Klipper (fork by memfis99999) — GD32 & Creality K1 Enhanced Firmware

[Original Klipper project](https://github.com/Klipper3d/klipper)


[Fork — memfis99999](https://github.com/memfis99999/klipper)

**This is a personal fork of [Klipper](https://github.com/Klipper3d/klipper) with extended support for GD32 MCUs and Creality K1 printers.**
Includes custom Makefile and Kconfig files for simplified firmware building and maximum compatibility with Creality K1 hardware.

---

## ⚠️ Disclaimer

- **This is NOT an official Klipper product.**
- Not affiliated with, maintained, or endorsed by the original Klipper developers.
- "Klipper" may be a registered trademark; it is used here only for compatibility and identification.
- All rights to the original Klipper project belong to their respective authors.
- This fork started in **2025** by [memfis99999](https://github.com/memfis99999).

---

## 💡 Overview

Klipper is an open-source 3D printer firmware that combines the power of a general-purpose computer (such as a Raspberry Pi) with one or more microcontrollers to achieve high-performance, flexible, and modular printer control. It offers advanced motion algorithms, easy configuration, and is widely adopted in the 3D printing community for its speed and feature set.

This fork introduces:

- **GD32 microcontroller support**
  Out-of-the-box support for GD32 MCUs for use in Creality K1 series printers.
- **Creality K1-specific features**
  All necessary Makefile and Kconfig changes for streamlined firmware builds on K1 hardware.
- **Maximum compatibility**
  Built to work with [KlipperLab](https://github.com/memfis99999/KlipperLab) environments, but also fully usable standalone.
- **Open for further Creality integrations**
  As new official Creality sources become available, support for additional hardware will be added.

**Note:**
Currently, prtouch support from Creality is NOT included, as available files are only compatible with very old Klipper forks.
Full reimplementation is planned.

---

##  ❗❗❗ prTouch removed ❗❗❗

The original Creality prTouch implementation has been fully removed from this fork. It relied on outdated and incompatible code from early Creality Klipper derivatives, making it impossible to maintain or integrate cleanly.

If your printer previously used prTouch for Z endstop detection, you must switch to alternative Z-probe or endstop methods, such as:

inductive or optical Z-probe

mechanical endstop

nozzle-based probing (Klicky, Euclid, etc.)

manual Z offset calibration

A full prTouch reimplementation is planned for the future, but the legacy version will not return.

---

## 📦 Installing Klipper + matching firmware (no compilation)


  This fork provides prebuilt MCU firmware for Creality K1 printers.
Firmware is built automatically by GitHub Actions for every commit in the master branch.
Each firmware package contains a text file with ready-to-run commands that install the exact Klipper version used during the build.
Installation consists of two steps:
1.  Clone Klipper
2.  Run the helper script that downloads the matching firmware

SSH into the printer and run:
```bash
git clone --branch master https://github.com/memfis99999/klipper.git klipper
cd klipper
./scripts/get-fw.sh
```

After this, the directory fw/K1/ will contain all MCU binaries corresponding to your current Klipper commit.
This fork is designed so that Klipper and MCU firmware can be updated together, without compiling anything on the printer.

Directory structure becomes:
```tree
klipper/
  src/
  klippy/
  fw/        (prebuilt MCU binaries)
    K1/
      dict/  (prebuild dictionary)
      bed0_100_G21-bed0_0xx_yyy.bin
      bed0_110_G21-bed0_0xx_yyy.bin
      mcu0_110_G32-mcu0_0xx_yyy.bin
      mcu0_110_S40-mcu0_0xx_yyy.bin
      mcu0_120_G32-mcu0_0xx_yyy.bin
      noz0_110_S06-noz0_0xx_yyy.bin
      noz0_110_G30-noz0_0xx_yyy.bin
      noz0_120_G30-noz0_0xx_yyy.bin
      firmware.txt
      build.log
  .gitmodules
```
### 🔄 Updating Klipper + MCU firmware (one command)

To update both Klipper and MCU firmware:
```bash
cd ~/klipper
git pull
./scripts/get-fw.sh

(reboot Creality 3D Printer)
```
git pull updates Klipper (branch master).



Klipper restarts and loads the new binaries.

No compilation on the printer.

### 🤖 How firmware is delivered (GitHub Actions automation)

The firmware-binaries branch "fw-files" is updated automatically:

Any commit in master triggers GitHub Actions.

Firmware is built in the cloud.

Binaries are copied into "fw-files" branch.

###  Advantages:

Firmware always matches the commit.

No dirty builds.

No commit → build → commit loops.

Works with Fluidd/Mainsail.

Easy rollback to any commit.

---

## 📝 License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**.
See [LICENSE](https://www.gnu.org/licenses/gpl-3.0.html) for details.

You are free to use, modify, distribute, and adapt this code
**provided you comply with the terms of the GPLv3.**

---

## 🔗 Resources

- [Original Klipper repository](https://github.com/Klipper3d/klipper) — please see for general documentation and history.
- [KlipperLab environment](https://github.com/memfis99999/KlipperLab) — recommended for batch builds and simulation.

---

**All contributions and feedback are welcome!**

— [memfis99999](https://github.com/memfis99999)
