#!/bin/bash
# Fetch matching firmware for current Klipper commit
set -e

MAIN_DIR=${PWD}
FW_DIR=${MAIN_DIR}/fw

KLIPPER_COMMIT=$(git rev-parse HEAD)
echo "Current Klipper commit: $KLIPPER_COMMIT"

TMPDIR=$(mktemp -d)
echo "Using temp directory: $TMPDIR"

git clone --branch fw-files --depth 50 https://github.com/memfis99999/klipper.git "$TMPDIR/fw-files"

cd "$TMPDIR/fw-files"
FW_COMMIT=$(git log --grep="FW for $KLIPPER_COMMIT" --format="%H" -n 1 || true)

if [ -z "$FW_COMMIT" ]; then
    echo "ERROR: No firmware commit found for Klipper commit $KLIPPER_COMMIT"
    exit 1
fi

echo "Found firmware commit: $FW_COMMIT"

git checkout "$FW_COMMIT" >/dev/null 2>&1
echo "Checked out firmware commit: $FW_COMMIT"

cd ${MAIN_DIR}
rm -rf ${FW_DIR}
mkdir -p ${FW_DIR}
cp -r "$TMPDIR/fw-files/fw/"* ${FW_DIR}/

echo "Firmware installed into ${FW_DIR}/"
echo "Done."

rm -rf "$TMPDIR"
