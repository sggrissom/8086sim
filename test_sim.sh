#!/bin/bash
set -e

CPU="./a.out"  # your decoder binary
ASM_DIR="asm"
TMP_DIR="tmp"
mkdir -p "$TMP_DIR"

PASSED=0
FAILED=0

for ASM_FILE in "$ASM_DIR"/*.asm; do
    NAME=$(basename "$ASM_FILE" .asm)
    BIN_FILE="$TMP_DIR/${NAME}.bin"
    OUT_ASM="$TMP_DIR/${NAME}_decoded.asm"
    REGEN_FILE="$TMP_DIR/${NAME}_regen.bin"

    echo "▶ Testing: $NAME"

    # 1. Assemble original
    nasm -f bin "$ASM_FILE" -o "$BIN_FILE"

    # 2. Run decoder
    $CPU "$BIN_FILE" > "$OUT_ASM"

    # 3. Assemble regenerated assembly
    nasm -f bin "$OUT_ASM" -o "$REGEN_FILE"

    # 4. Compare binaries
    if cmp -s "$BIN_FILE" "$REGEN_FILE"; then
        echo "✅ PASS: $NAME"
        ((PASSED++))
    else
        echo "❌ FAIL: $NAME"
        ((FAILED++))

        # Show diff if failed
        echo "--- Original ($BIN_FILE)"
        xxd "$BIN_FILE" > "$TMP_DIR/${NAME}_orig.hex"
        echo "--- Regenerated ($REGEN_FILE)"
        xxd "$REGEN_FILE" > "$TMP_DIR/${NAME}_regen.hex"

        diff -u "$TMP_DIR/${NAME}_orig.hex" "$TMP_DIR/${NAME}_regen.hex" || true
    fi
done

echo ""
echo "=== Summary ==="
echo "Passed: $PASSED"
echo "Failed: $FAILED"

