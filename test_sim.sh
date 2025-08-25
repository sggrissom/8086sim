#!/bin/bash
set -e

CPU_SRC="./src/8086sim.cpp"
CPU_BIN="./8086sim"
ASM_DIR="asm"
TMP_DIR="tmp"
mkdir -p "$TMP_DIR"

echo "🔧 Building sim from $CPU_SRC..."
g++ "$CPU_SRC" -o "$CPU_BIN"

set +e

PASSED=0
FAILED=0

echo "$ASM_DIR"/*.asm

strip_comments() {
    sed -E '/^\s*;/d' "$1" |      # delete lines starting with ;
    sed -E 's/\s*;.*$//' |       # remove inline comments
    sed -E '/^\s*$/d'            # delete empty lines
}


for ASM_FILE in "$ASM_DIR"/*.asm; do
    NAME=$(basename "$ASM_FILE" .asm)
    BIN_FILE="$TMP_DIR/${NAME}.bin"
    OUT_ASM="$TMP_DIR/${NAME}_decoded.asm"
    REGEN_FILE="$TMP_DIR/${NAME}_regen.bin"

    echo "▶ Testing: $NAME"

    # 1. Assemble original
    nasm -f bin "$ASM_FILE" -o "$BIN_FILE"

    # 2. Run decoder
    "$CPU_BIN" "$BIN_FILE" > "$OUT_ASM"

    # 3. Assemble regenerated assembly
    nasm -f bin "$OUT_ASM" -o "$REGEN_FILE"

    # 4. Compare binaries
    if cmp -s "$BIN_FILE" "$REGEN_FILE"; then
        echo "✅ PASS: $NAME"
        ((PASSED++))
    else
        echo "❌ FAIL: $NAME"
        ((FAILED++))
        if [[ -n "$CI" ]]; then
          echo "skip diff"
        else
            # Show diff if failed
            colordiff -u <(strip_comments "$ASM_FILE") "$OUT_ASM" || true
        fi
        exit
fi
done

echo ""
echo "=== Summary ==="
echo "Passed: $PASSED"
echo "Failed: $FAILED"

exit $FAILED
