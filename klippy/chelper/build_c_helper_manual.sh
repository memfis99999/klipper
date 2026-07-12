#!/bin/sh
set -eu

cd /usr/data/klipper/klippy/chelper

CC=/opt/bin/gcc
LD=/opt/bin/ld

SRC="
pyhelper.c
serialqueue.c
stepcompress.c
steppersync.c
itersolve.c
trapq.c
pollreactor.c
msgblock.c
trdispatch.c
kin_cartesian.c
kin_corexy.c
kin_corexz.c
kin_delta.c
kin_deltesian.c
kin_polar.c
kin_rotary_delta.c
kin_winch.c
kin_extruder.c
kin_shaper.c
kin_idex.c
kin_generic.c
"

OBJDIR=.obj-k1c
OUT=c_helper.so.manual

rm -rf "$OBJDIR"
mkdir -p "$OBJDIR"
rm -f "$OUT"

CFLAGS="
-O2
-g
-Wall
-fPIC
-fno-lto
-fno-use-linker-plugin
-fno-plt
-fno-exceptions
-fno-unwind-tables
-fno-asynchronous-unwind-tables
-U_FORTIFY_SOURCE
-D_FORTIFY_SOURCE=0
"

INCLUDES="-I."

echo "[1/2] compile objects"
for f in $SRC; do
    o="$OBJDIR/$(basename "$f" .c).o"
    echo "  CC $f"
    $CC $CFLAGS $INCLUDES -c "$f" -o "$o"
done

echo "[2/2] link shared object"
# Линкуем вручную через ld, чтобы не получить libstdc++ и RPATH=/opt/lib
# -lc оставляем, потому что старый рабочий c_helper.so зависел только от libc.so.6
$LD -shared \
    -soname c_helper.so \
    -o "$OUT" \
    $OBJDIR/*.o \
    -lc

chmod 755 "$OUT"

echo
echo "=== readelf -d $OUT ==="
readelf -d "$OUT" | grep -E 'NEEDED|RPATH|RUNPATH' || true

echo
echo "=== undefined/global symbols of interest ==="
readelf -Ws "$OUT" | grep -E 'steppersyncmgr_alloc|steppersyncmgr_free|steppersyncmgr_alloc_steppersync|steppersyncmgr_gen_steps' || true

echo
echo "=== strings suspicious paths ==="
strings "$OUT" | grep -E '/opt/|libstdc\\+\\+|mipsel-openwrt' || true

echo
echo "Built: $(pwd)/$OUT"
