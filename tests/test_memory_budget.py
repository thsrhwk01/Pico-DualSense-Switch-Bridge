#!/usr/bin/env python3
"""Regression cases for the release heap guard; no Pico SDK needed."""

import importlib.util
from pathlib import Path
import unittest


spec = importlib.util.spec_from_file_location(
    "memory_budget", Path(__file__).resolve().parents[1] / "tools/check_memory_budget.py")
budget = importlib.util.module_from_spec(spec)
spec.loader.exec_module(budget)


class MemoryBudgetTests(unittest.TestCase):
    def symbols(self, free, audio=True):
        symbols = {"__end__": 0x20080000 - free, "__HeapLimit": 0x20080000,
                   "__StackLimit": 0x20080000}
        if audio:
            symbols["opus_encoder_create"] = 0x20014470
        return symbols

    def test_v081_release_layout_is_rejected(self):
        # Values recovered from the actual downloaded release UF2 startup data.
        with self.assertRaisesRegex(ValueError, "105,100 bytes available"):
            budget.check_budget(self.symbols(105100), "audio")

    def test_old_working_image_also_needs_more_margin(self):
        with self.assertRaisesRegex(ValueError, "115,444 bytes available"):
            budget.check_budget(self.symbols(115444), "audio")

    def test_minimum_reserve_and_one_byte_short(self):
        for profile, required in budget.MINIMUM_HEAP.items():
            with self.subTest(profile=profile):
                budget.check_budget(self.symbols(required, profile == "audio"), profile)
                with self.assertRaises(ValueError):
                    budget.check_budget(self.symbols(required - 1, profile == "audio"), profile)

    def test_no_speaker_profile_cannot_hide_audio_regression(self):
        with self.assertRaisesRegex(ValueError, "does not match"):
            budget.check_budget(self.symbols(105100), "no-speaker")

    def test_sbrk_stack_limit_is_respected(self):
        symbols = self.symbols(128 * 1024)
        symbols["__StackLimit"] -= 4
        with self.assertRaisesRegex(ValueError, "131,068 bytes available"):
            budget.check_budget(symbols, "audio")

    def test_missing_symbols_and_invalid_bounds_fail_closed(self):
        for symbols in ({}, {"__end__": 0x20000000},
                        {"__end__": 0x20090000, "__HeapLimit": 0x20080000,
                         "__StackLimit": 0x20080000},
                        {"__end__": 0x10000000, "__HeapLimit": 0x10080000,
                         "__StackLimit": 0x10080000}):
            with self.subTest(symbols=symbols), self.assertRaises(ValueError):
                budget.check_budget(symbols, "audio")

    def test_gnu_nm_output(self):
        symbols = budget.parse_symbols(
            "__end__ B 20060000 \n__HeapLimit B 20080000 \n"
            "__StackLimit R 20080000 \n"
            "opus_encoder_create T 20014470 134\n")
        budget.check_budget(symbols, "audio")


if __name__ == "__main__":
    unittest.main()
