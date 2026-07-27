#!/usr/bin/env python3
"""Remove legacy Zcash hashActivationBlock assignments from chainparams.cpp."""
import re
from pathlib import Path

path = Path(__file__).resolve().parents[2] / "src" / "chainparams.cpp"
text = path.read_text(encoding="utf-8")
pattern = (
    r"\n        consensus\.vUpgrades\[Consensus::UPGRADE_[^\]]+\]"
    r"\.hashActivationBlock =\n            uint256S\(\"[^\"]+\"\);"
)
new_text, n = re.subn(pattern, "", text)
if n == 0:
    raise SystemExit("No hashActivationBlock assignments removed")
path.write_text(new_text, encoding="utf-8")
print(f"Removed {n} hashActivationBlock assignments from {path}")
