#!/usr/bin/env python3
"""Generate deterministic Spectre P2SH placeholder addresses for chainparams."""
import hashlib

B58_ALPHABET = b"123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz"


def b58encode(data: bytes) -> str:
    n = int.from_bytes(data, "big")
    chars = []
    while n:
        n, r = divmod(n, 58)
        chars.append(B58_ALPHABET[r : r + 1])
    for b in data:
        if b == 0:
            chars.append(B58_ALPHABET[0:1])
        else:
            break
    return b"".join(reversed(chars)).decode()


def addr(prefix: bytes, tag: str, i: int = 0) -> str:
    h = hashlib.sha256(f"{tag}:{i}".encode()).digest()[:20]
    data = prefix + h
    chk = hashlib.sha256(hashlib.sha256(data).digest()).digest()[:4]
    return b58encode(data + chk)


MAIN_SCRIPT = bytes([0x1C, 0x2C])
TEST_SCRIPT = bytes([0x1C, 0xBA])


def main() -> None:
    print("=== MAIN founders (48) ===")
    for i in range(48):
        print(f'            "{addr(MAIN_SCRIPT, "spectre-founder", i)}",')

    print("\n=== MAIN bp (51) ===")
    for i in range(51):
        print(f'                "{addr(MAIN_SCRIPT, "spectre-bp", i)}",')

    print("\n=== MAIN singles ===")
    print("zf:", addr(MAIN_SCRIPT, "spectre-zf", 0))
    print("mg:", addr(MAIN_SCRIPT, "spectre-mg", 0))
    print("fpf:", addr(MAIN_SCRIPT, "spectre-fpf", 0))
    print("kho:", addr(MAIN_SCRIPT, "spectre-kho", 0))

    print("\n=== TEST founders ===")
    print(addr(TEST_SCRIPT, "spectre-test-founder", 0))

    print("\n=== TEST bp (51) ===")
    for i in range(51):
        print(f'                "{addr(TEST_SCRIPT, "spectre-test-bp", i)}",')

    print("\n=== TEST singles ===")
    print("zf:", addr(TEST_SCRIPT, "spectre-test-zf", 0))
    print("mg:", addr(TEST_SCRIPT, "spectre-test-mg", 0))
    print("fpf:", addr(TEST_SCRIPT, "spectre-test-fpf", 0))
    print("kho:", addr(TEST_SCRIPT, "spectre-test-kho", 0))


if __name__ == "__main__":
    main()
