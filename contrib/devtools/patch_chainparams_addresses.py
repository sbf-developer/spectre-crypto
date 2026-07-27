#!/usr/bin/env python3
"""Patch src/chainparams.cpp with generated Spectre placeholder addresses."""
from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHAINPARAMS = ROOT / "src" / "chainparams.cpp"
ADDRS = Path(__file__).with_name("spectre_addresses.txt")


def quoted_lines(entries: list[str], indent: str) -> str:
    return "\n".join(f'{indent}"{e}",' for e in entries)


def parse_addresses(text: str) -> dict:
    data: dict = {
        "main_founders": [],
        "main_bp": [],
        "main": {},
        "test_founders": "",
        "test_bp": [],
        "test": {},
    }
    section = None
    for line in text.splitlines():
        line = line.lstrip("\ufeff")
        if not line.strip():
            continue
        if line.startswith("=== MAIN founders"):
            section = "main_founders"
        elif line.startswith("=== MAIN bp"):
            section = "main_bp"
        elif line.startswith("=== MAIN singles"):
            section = "main_singles"
        elif line.startswith("=== TEST founders"):
            section = "test_founders_line"
        elif line.startswith("=== TEST bp"):
            section = "test_bp"
        elif line.startswith("=== TEST singles"):
            section = "test_singles"
        elif m := re.match(r'\s+"([^"]+)",', line):
            if section not in data or not isinstance(data[section], list):
                continue
            data[section].append(m.group(1))
        elif m := re.match(r"(zf|mg|fpf|kho): (.+)", line):
            bucket = "main" if section == "main_singles" else "test"
            data[bucket][m.group(1)] = m.group(2)
        elif section == "test_founders_line" and line.strip() and not line.startswith("==="):
            data["test_founders"] = line.strip()
    return data


def patch_main(section: str, addrs: dict) -> str:
    section = replace_vector(section, "bp_addresses", quoted_lines(addrs["main_bp"], "                "))
    section = replace_repeat(section, "zf_addresses", 48, addrs["main"]["zf"])
    section = replace_repeat(section, "mg_addresses", 48, addrs["main"]["mg"])
    section = replace_repeat(section, "fpf_addresses", 12, addrs["main"]["fpf"])
    section = replace_repeat(section, "fpf_addresses_h3", 36, addrs["main"]["fpf"])
    section = replace_string_var(section, "nu6_1_kho_address", addrs["main"]["kho"])
    section = replace_founders(section, quoted_lines(addrs["main_founders"], "            "))
    section = replace_checkpoint_main(section)
    section = replace_supply_checkpoints(section)
    return section


def patch_test(section: str, addrs: dict) -> str:
    section = replace_test_identity(section)
    section = replace_vector(section, "bp_addresses", quoted_lines(addrs["test_bp"], "                "))
    section = replace_repeat(section, "zf_addresses", 51, addrs["test"]["zf"])
    section = replace_repeat(section, "mg_addresses", 51, addrs["test"]["mg"])
    section = replace_repeat(section, "fpf_addresses", 13, addrs["test"]["fpf"])
    section = replace_repeat(section, "fpf_addresses_h3", 27, addrs["test"]["fpf"])
    section = replace_string_var(section, "nu6_1_kho_address", addrs["test"]["kho"])
    section = re.sub(
        r"vFoundersRewardAddress = \{[^}]+\};",
        f'vFoundersRewardAddress = {{ "{addrs["test_founders"]}" }};',
        section,
        count=1,
    )
    section = replace_checkpoint_test(section)
    section = replace_supply_checkpoints(section)
    return section


def replace_vector(section: str, var_name: str, body: str) -> str:
    pattern = rf"(std::vector<std::string> {var_name} = \{{\n)(.*?)(\n\s*\}};)"
    new, n = re.subn(pattern, rf"\g<1>{body}\n            }};", section, count=1, flags=re.DOTALL)
    if n != 1:
        raise RuntimeError(f"vector {var_name}")
    return new


def replace_repeat(section: str, var_name: str, count: int, address: str) -> str:
    pattern = rf'std::vector<std::string> {var_name}\({count}, "[^"]+"\);'
    new, n = re.subn(pattern, f'std::vector<std::string> {var_name}({count}, "{address}");', section, count=1)
    if n != 1:
        raise RuntimeError(f"repeat {var_name}({count})")
    return new


def replace_string_var(section: str, var_name: str, value: str) -> str:
    new, n = re.subn(rf'std::string {var_name} = "[^"]+";', f'std::string {var_name} = "{value}";', section, count=1)
    if n != 1:
        raise RuntimeError(var_name)
    return new


def replace_founders(section: str, body: str) -> str:
    new, n = re.subn(
        r"vFoundersRewardAddress = \{\n.*?\n        \};",
        f"vFoundersRewardAddress = {{\n{body}\n        }};",
        section,
        count=1,
        flags=re.DOTALL,
    )
    if n != 1:
        raise RuntimeError("founders")
    return new


def replace_checkpoint_main(section: str) -> str:
    new, n = re.subn(
        r"checkpointData = \(CCheckpointData\) \{\n            boost::assign::map_list_of\n.*?\n        \};",
        """checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock),
            0,
            0,
            0
        };""",
        section,
        count=1,
        flags=re.DOTALL,
    )
    if n != 1:
        raise RuntimeError("main checkpoint")
    return new


def replace_checkpoint_test(section: str) -> str:
    new, n = re.subn(
        r"checkpointData = \(CCheckpointData\) \{\n            boost::assign::map_list_of\n.*?\n        \};",
        """checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock),
            0,
            0,
            0
        };""",
        section,
        count=1,
        flags=re.DOTALL,
    )
    if n != 1:
        raise RuntimeError("test checkpoint")
    return new


def replace_supply_checkpoints(section: str) -> str:
    return re.sub(
        r"// Hardcoded fallback value for the Sprout shielded value pool balance.*?hashChainSupplyCheckpointBlock = uint256S\(\"[^\"]+\"\);",
        """// Fresh Spectre chain: no legacy value-pool or supply checkpoints.
        nSproutValuePoolCheckpointHeight = 0;
        nSproutValuePoolCheckpointBalance = 0;
        fZIP209Enabled = true;
        hashSproutValuePoolCheckpointBlock.SetNull();
        nChainSupplyCheckpointHeight = 0;
        nChainSupplyCheckpointTotalSupply = 0;
        nChainSupplyCheckpointTransparentValue = 0;
        nChainSupplyCheckpointSproutValue = 0;
        nChainSupplyCheckpointSaplingValue = 0;
        nChainSupplyCheckpointOrchardValue = 0;
        nChainSupplyCheckpointLockboxValue = 0;
        hashChainSupplyCheckpointBlock.SetNull();""",
        section,
        count=1,
        flags=re.DOTALL,
    )


def replace_test_identity(section: str) -> str:
    section = re.sub(
        r"keyConstants\.bip44CoinType = 1;",
        "keyConstants.bip44CoinType = 19118; // Spectre testnet",
        section,
        count=1,
    )
    section = re.sub(
        r'keyConstants\.bech32HRPs\[SAPLING_PAYMENT_ADDRESS\]\s+= "ztestsapling";',
        'keyConstants.bech32HRPs[SAPLING_PAYMENT_ADDRESS]      = "tspec";',
        section,
        count=1,
    )
    section = re.sub(
        r'keyConstants\.bech32HRPs\[SAPLING_FULL_VIEWING_KEY\]\s+= "zviewtestsapling";',
        'keyConstants.bech32HRPs[SAPLING_FULL_VIEWING_KEY]     = "tspecviews";',
        section,
        count=1,
    )
    section = re.sub(
        r'keyConstants\.bech32HRPs\[SAPLING_INCOMING_VIEWING_KEY\]\s+= "zivktestsapling";',
        'keyConstants.bech32HRPs[SAPLING_INCOMING_VIEWING_KEY] = "tspecivks";',
        section,
        count=1,
    )
    section = re.sub(
        r'keyConstants\.bech32HRPs\[SAPLING_EXTENDED_SPEND_KEY\]\s+= "secret-extended-key-test";',
        'keyConstants.bech32HRPs[SAPLING_EXTENDED_SPEND_KEY]   = "secret-extended-key-tspec";',
        section,
        count=1,
    )
    section = re.sub(
        r'keyConstants\.bech32HRPs\[SAPLING_EXTENDED_FVK\]\s+= "zxviewtestsapling";',
        'keyConstants.bech32HRPs[SAPLING_EXTENDED_FVK]         = "txspecviews";',
        section,
        count=1,
    )
    section = re.sub(
        r'keyConstants\.bech32mHRPs\[TEX_ADDRESS\]\s+= "textest";',
        'keyConstants.bech32mHRPs[TEX_ADDRESS]                 = "tstex";',
        section,
        count=1,
    )
    section = re.sub(
        r"consensus\.nMinimumChainWork = uint256S\(\"000000000000000000000000000000000000000000000000000000263c0984a2\"\);",
        'consensus.nMinimumChainWork = uint256S("0x00");',
        section,
        count=1,
    )
    section = re.sub(
        r"pchMessageStart\[0\] = 0xfa;\n        pchMessageStart\[1\] = 0x1a;\n        pchMessageStart\[2\] = 0xf9;\n        pchMessageStart\[3\] = 0xbf;\n        nDefaultPort = 18233;",
        """// Spectre testnet P2P magic ("TSPC")
        pchMessageStart[0] = 0x54;
        pchMessageStart[1] = 0x53;
        pchMessageStart[2] = 0x50;
        pchMessageStart[3] = 0x43;
        nDefaultPort = 18733;""",
        section,
        count=1,
    )
    return section


def main() -> None:
    addrs = parse_addresses(ADDRS.read_text(encoding="utf-8"))
    content = CHAINPARAMS.read_text(encoding="utf-8")

    main_start = content.index("class CMainParams")
    test_start = content.index("class CTestNetParams")
    reg_start = content.index("class CRegTestParams")

    main = content[main_start:test_start]
    test = content[test_start:reg_start]

    main = patch_main(main, addrs)
    test = patch_test(test, addrs)

    CHAINPARAMS.write_text(content[:main_start] + main + test + content[reg_start:], encoding="utf-8")
    print("Patched", CHAINPARAMS)


if __name__ == "__main__":
    main()
