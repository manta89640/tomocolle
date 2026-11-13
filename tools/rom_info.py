#!/usr/bin/env python3
"""Prints what a DS ROM says about itself: its header, its programs and overlays, and the SDK it was built with.

Reads the ROM directly, without dsd, so the facts in docs/binary.md can be checked against the ROM alone.
"""
import argparse
import collections
import hashlib
import re
import struct
import sys
from pathlib import Path

NITROCODE = bytes.fromhex("2106c0de" "dec00621")


def u32(data: bytes, offset: int) -> int:
    return struct.unpack_from("<I", data, offset)[0]


def decompress(data: bytes) -> bytes:
    """Decompresses the SDK's backward LZ ("BLZ"), which a program or overlay is compressed with from its end."""
    extra = u32(data, len(data) - 4)
    if extra == 0:
        return data
    header = u32(data, len(data) - 8)
    header_size, compressed_size = header >> 24, header & 0xFFFFFF
    keep = len(data) - compressed_size
    source = data[keep:len(data) - header_size][::-1]
    size = compressed_size + extra
    out = bytearray()
    i = 0
    while len(out) < size:
        flags = source[i]
        i += 1
        for bit in range(8):
            if len(out) >= size:
                break
            if flags & (0x80 >> bit):
                pair = (source[i] << 8) | source[i + 1]
                i += 2
                distance = (pair & 0xFFF) + 3
                for _ in range((pair >> 12) + 3):
                    out.append(out[-distance])
            else:
                out.append(source[i])
                i += 1
    return data[:keep] + bytes(out[::-1])


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("rom", type=Path)
    parser.add_argument("--strings", action="store_true", help="also print the SDK and middleware version strings")
    args = parser.parse_args()
    rom = args.rom.read_bytes()

    print(f"SHA-1         {hashlib.sha1(rom).hexdigest()}")
    print(f"Title         {rom[0:12].rstrip(bytes(1)).decode('ascii')}")
    print(f"Game code     {rom[12:16].decode('ascii')}, maker {rom[16:18].decode('ascii')}")
    print(f"Unit code     {rom[0x12]}")
    print(f"ROM version   {rom[0x1E]}")
    print(f"Size          {len(rom):#x}, {u32(rom, 0x80):#x} used")

    arm9_offset, arm9_entry, arm9_address, arm9_size = struct.unpack_from("<4I", rom, 0x20)
    arm7_offset, arm7_entry, arm7_address, arm7_size = struct.unpack_from("<4I", rom, 0x30)
    arm9 = rom[arm9_offset:arm9_offset + arm9_size]
    print(f"Secure area   {'decrypted' if arm9[:8] == bytes.fromhex('ffdeffe7ffdeffe7') else 'encrypted or absent'}")

    # The module parameters end with the "nitrocode", and hold where the autoloads and the compressed part are
    params = arm9.find(NITROCODE)
    if params < 28:
        print("No module parameters found", file=sys.stderr)
        return 1
    autoload_list, autoload_list_end, _, bss_start, bss_end, compressed_end, sdk = struct.unpack_from(
        "<7I", arm9, params - 28)
    if compressed_end:
        split = compressed_end - arm9_address
        arm9 = decompress(arm9[:split]) + arm9[split:]
    print(f"SDK version   {sdk >> 24}.{(sdk >> 16) & 0xFF}.{sdk & 0xFFFF} ({sdk:#010x})")
    print(f"ARM9          at {arm9_address:#010x}, entry {arm9_entry:#010x}, {arm9_size:#x} bytes in the ROM, "
          f"{len(arm9):#x} decompressed")
    print(f"ARM9 BSS      {bss_start:#010x} to {bss_end:#010x}")
    for entry in range(autoload_list - arm9_address, autoload_list_end - arm9_address, 12):
        address, size, bss_size = struct.unpack_from("<3I", arm9, entry)
        print(f"Autoload      at {address:#010x}, {size:#x} bytes, {bss_size:#x} of BSS")
    print(f"ARM7          at {arm7_address:#010x}, entry {arm7_entry:#010x}, {arm7_size:#x} bytes")

    table_offset, table_size, fat_offset = u32(rom, 0x50), u32(rom, 0x54), u32(rom, 0x48)
    modules = {"main": arm9}
    addresses = collections.Counter()
    compressed = 0
    for i in range(table_size // 32):
        overlay, address, _, _, _, _, file_id, flags = struct.unpack_from("<8I", rom, table_offset + i * 32)
        start, end = struct.unpack_from("<2I", rom, fat_offset + file_id * 8)
        data = rom[start:end]
        if (flags >> 24) & 1:
            data = decompress(data[:flags & 0xFFFFFF])
            compressed += 1
        modules[f"ov{overlay:03}"] = data
        addresses[address] += 1
    total = sum(len(data) for name, data in modules.items() if name != "main")
    print(f"ARM9 overlays {len(modules) - 1}, {compressed} compressed, {total:#x} bytes decompressed")
    for address, count in addresses.most_common():
        print(f"              {count} at {address:#010x}")

    if args.strings:
        tags = re.compile(rb"\[SDK\+[\x20-\x7e]+?\]")
        for name, data in modules.items():
            for match in tags.finditer(data):
                print(f"String        {match.group().decode('ascii')} in {name}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
