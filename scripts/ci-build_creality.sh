#!/bin/bash
# Firmware build script for Creality K1 family (CI + local).
# Intended to be run from Klipper repository root after ./scripts/ci-install.sh

set -eu
set -o pipefail

SECONDS=0

MAIN_DIR=${PWD}
BUILD_DIR=${PWD}/ci_build
OUT_DIR=${PWD}/out
ARTIFACT_DIR=${PWD}/output
LOG_FILE=${ARTIFACT_DIR}/build.log
DICTDIR=${ARTIFACT_DIR}/dict
FW_DESCRIPTION_FILE=${ARTIFACT_DIR}/firmware.txt

CONFIGS_DIR=${PWD}/src/configs/Creality

rm -rf "${OUT_DIR}" "${ARTIFACT_DIR}" "${DICTDIR}"

mkdir -p "${OUT_DIR}" "${ARTIFACT_DIR}" "${DICTDIR}"

# Get full version from Git
GIT_DESCRIBE=$(git describe --tags --long --dirty --always)
VERSION_FULL="${GIT_DESCRIBE}"
echo "VERSION_FULL=${VERSION_FULL}"

# Parse version information
if [[ $VERSION_FULL =~ ^[vV]([0-9]+)\.([0-9]+)\.([0-9]+)-([0-9]+)-g([a-f0-9]+)(-dirty)? ]]; then
    # Clasic: MAJOR.MINOR.PATCH
    MAJOR="${BASH_REMATCH[1]}"
    MINOR="${BASH_REMATCH[2]}"
    PATCH="${BASH_REMATCH[3]}"
    COMMITS="${BASH_REMATCH[4]}"
    GIT_HASH="${BASH_REMATCH[5]}"
    DIRTY="${BASH_REMATCH[6]}"
elif [[ $VERSION_FULL =~ ^[vV]([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)-([0-9]+)-g([a-f0-9]+)(-dirty)? ]]; then
    # Creality: MAJOR.MINOR.PATCH.BUILD
    MAJOR="${BASH_REMATCH[1]}"
    MINOR="${BASH_REMATCH[2]}"
    PATCH="${BASH_REMATCH[3]}"
    BUILD="${BASH_REMATCH[4]}"
    COMMITS="${BASH_REMATCH[5]}"
    GIT_HASH="${BASH_REMATCH[6]}"
    DIRTY="${BASH_REMATCH[7]}"
else
  echo "ERROR: Failed to parse version: ${VERSION_FULL}" | tee -a "${LOG_FILE}"
  exit 1
    MAJOR="0"
    MINOR="0"
    PATCH="0"
    BUILD="0"
    COMMITS="0"
    GIT_HASH="0"
    DIRTY=""
fi

# Get repository URL
REPO_URL=$(git config --get remote.origin.url)
REPO_CLEANED=$(echo "${REPO_URL}" | sed -E 's#(git@|https://)github.com[:/]##; s/.git$//')
GITHUB_URL="https://github.com/${REPO_CLEANED}"
GIT_BRANCH="$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo 'N/A')"

time_now="$(date '+%Y-%m-%d %H:%M:%S')"
{
  echo "----------------------------------------"
  echo "[${time_now}] 🛠️ Build firmware (Creality_K1)"
  echo "[${time_now}] 🛠️ Version: ${VERSION_FULL}"
  echo "[${time_now}] 🛠️ Repo: ${GITHUB_URL}"
  echo "[${time_now}] 🛠️ Branch: ${GIT_BRANCH}"
  echo "[${time_now}] 🛠️ CONFIGS_DIR: ${CONFIGS_DIR}"
} >> "${LOG_FILE}"

# Validate configs dir
if [ ! -d "${CONFIGS_DIR}" ]; then
  echo "ERROR: CONFIGS_DIR not found: ${CONFIGS_DIR}" | tee -a "${LOG_FILE}"
  exit 2
fi

# Build all defconfigs
found_any=0
for filepath in "${CONFIGS_DIR}"/*_defconfig; do
  if [ ! -f "${filepath}" ]; then
    continue
  fi
  found_any=1

  filename="$(basename "${filepath}")"

  base_name="${filename#K*_}"
  base_name="${base_name%_defconfig}"

  time_now="$(date '+%Y-%m-%d %H:%M:%S')"
  echo "[${time_now}] Found config: ${filename}" | tee -a "${LOG_FILE}"

  # run build; on failure rerun verbose
  set +e

  make clean
  make distclean
  unset CC
  cp ${filepath} .config
  make olddefconfig
# CONFIG_MCU_BOARD_FW_RESERVED="$(printf "%03d" "${PATCH}")"
  make \
    CONFIG_MCU_BOARD_FW_VER="$(printf "%01d%02d" "${MAJOR}" "${MINOR}")" \
    CONFIG_MCU_BOARD_FW_RESERVED=$(printf "%03d" $((COMMITS % 1000))) \
    >> "${LOG_FILE}" 2>&1
  rc=$?
  set -e

  if [ $rc -ne 0 ]; then
    time_now="$(date '+%Y-%m-%d %H:%M:%S')"
    echo "[${time_now}] ❌ Build failed for ${filename}. Re-run with V=1" | tee -a "${LOG_FILE}"
    make clean >> "${LOG_FILE}" 2>&1 || true
    make \
      CONFIG_MCU_BOARD_FW_VER="$(printf "%01d%02d" "${MAJOR}" "${MINOR}")" \
      CONFIG_MCU_BOARD_FW_RESERVED=$(printf "%03d" $((COMMITS % 1000))) \
      V=1 >> "${LOG_FILE}" 2>&1
    exit 1
  fi

  # Collect .bin + dict
  fullpath="$(ls "out/${base_name}"*.bin 2>/dev/null | head -n 1 || true)"
  if [ -z "${fullpath}" ]; then
    echo "ERROR: .bin not found for ${filename} (expected: out/${base_name}*.bin)" | tee -a "${LOG_FILE}"
    exit 1
  fi

  filename_no_ext="$(basename "${fullpath}" .bin)"

  ./scripts/check-software-div.sh .config out/*.elf
  cp "out/${base_name}"*.bin "${ARTIFACT_DIR}/"
  cp "out/klipper.dict" "${DICTDIR}/${filename_no_ext}.dict"
done

if [ $found_any -eq 0 ]; then
  echo "ERROR: No *_defconfig files found in ${CONFIGS_DIR}" | tee -a "${LOG_FILE}"
  exit 2
fi

TOOLCHAIN_LINE="$(arm-none-eabi-gcc --version 2>/dev/null | head -1 || echo 'arm-none-eabi-gcc: N/A')"
time_now="$(date '+%Y-%m-%d %H:%M:%S')"

{
  echo "[${time_now}]"
  echo "🛠️ Build completed successfully."
  echo "🛠️ Firmware version: ${VERSION_FULL}"
  echo "🛠️ Repository: ${GITHUB_URL}"
  echo "🛠️ Branch: ${GIT_BRANCH}"
  echo "🛠️ Toolchain: ${TOOLCHAIN_LINE}"
  echo "🛠️ Artifacts saved to: ${ARTIFACT_DIR}"
  echo "🔧 Commands to install matching Klipper and firmware:"
  echo
  echo "git clone --branch ${GIT_BRANCH} ${GITHUB_URL}.git klipper && \\"
  echo "cd klipper && \\"
  echo "git checkout ${GIT_HASH} && \\"
  echo "./scripts/get-fw.sh"
} > "${FW_DESCRIPTION_FILE}"

echo "[${time_now}] 🛠️ Build completed successfully." | tee -a "${LOG_FILE}"
echo "Build took ${SECONDS} seconds." | tee -a "${LOG_FILE}"
echo "----------------------------------------" >> "${LOG_FILE}"
