#!/usr/bin/env python3
"""Reject linked firmware without enough heap for runtime initialization.

v0.8.1's standard UF2 had 105,100 bytes between __end__ and __HeapLimit.
Its startup allocations requested 101,271 bytes, but newlib's bookkeeping
and page-sized heap growth made Opus decoder creation panic. Full audio
builds must leave at least 128 KiB (about 29 KiB above those raw requests).
No-speaker builds omit the encoder, decoder and audio queue; reserve 32 KiB
for their approximately 14 KiB of startup allocations and later growth.

These are build-time reserves, not measurements of worst-case runtime use.
Hardware audio, reconnect and USB tests are still required.
"""

import argparse
import subprocess
import sys


MINIMUM_HEAP = {"audio": 128 * 1024, "no-speaker": 32 * 1024}


def parse_symbols(output):
    """Read GNU nm --defined-only --format=posix output."""
    symbols = {}
    for line in output.splitlines():
        fields = line.split()
        if len(fields) >= 3:
            symbols[fields[0]] = int(fields[2], 16)
    return symbols


def check_budget(symbols, profile):
    for name in ("__end__", "__HeapLimit", "__StackLimit"):
        if name not in symbols:
            raise ValueError("missing linker symbol: " + name)
    # Pico SDK newlib _sbrk uses __StackLimit as its upper bound.
    start = symbols["__end__"]
    limit = min(symbols["__HeapLimit"], symbols["__StackLimit"])
    if not 0x20000000 <= start <= limit <= 0x20100000:
        raise ValueError(f"invalid heap bounds: 0x{start:08x}..0x{limit:08x}")
    # Do not accidentally apply the smaller budget to a full-audio ELF.
    has_encoder = "opus_encoder_create" in symbols
    if has_encoder != (profile == "audio"):
        raise ValueError("memory profile does not match the linked Opus encoder")
    available = limit - start
    required = MINIMUM_HEAP[profile]
    report = (f"{profile} heap: {available:,} bytes available, "
              f"{required:,} required (0x{start:08x}..0x{limit:08x})")
    if available < required:
        raise ValueError(report + "; reduce static RAM use before distributing this UF2")
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nm", required=True, help="ARM toolchain nm executable")
    parser.add_argument("--profile", choices=MINIMUM_HEAP, default="audio")
    parser.add_argument("elf", help="linked, unstripped firmware ELF")
    args = parser.parse_args(argv)
    try:
        result = subprocess.run(
            [args.nm, "--defined-only", "--format=posix", args.elf],
            check=True, capture_output=True, text=True)
        report = check_budget(parse_symbols(result.stdout), args.profile)
    except (OSError, subprocess.CalledProcessError, ValueError) as error:
        print(f"Memory budget FAILED: {error}", file=sys.stderr)
        return 1
    print("Memory budget OK: " + report)
    return 0


if __name__ == "__main__":
    sys.exit(main())
