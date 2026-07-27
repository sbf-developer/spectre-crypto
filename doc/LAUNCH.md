# Spectre launch checklist

Use this after building binaries (`./zcutil/build.sh`). Code prep alone does not launch a network.

## 1. Build & verify

```bash
./zcutil/fetch-params.sh
./zcutil/build.sh -j$(nproc)
src/spectred -version
src/test/test_bitcoin --list_content  # optional: run unit tests
```

## 2. Replace placeholder addresses

1. Generate real 2-of-3 multisig transparent addresses on **mainnet** params (`s3...`).
2. Replace `vFoundersRewardAddress` and ZIP 214 address vectors in `src/chainparams.cpp`.
3. Regenerate or hand-edit; do **not** ship placeholder addresses from `gen_spectre_addresses.py` on mainnet.
4. Rebuild.

## 3. Choose genesis strategy

**Option A — Shared Zcash genesis (current):** Fastest path. Spectre mainnet diverges at block 1 once the first Spectre block is mined. Still requires all nodes to use Spectre magic/ports/HRPs.

**Option B — New genesis:** Mine a new Equihash `(200,9)` genesis block, update `CreateGenesisBlock` args and hash asserts in `chainparams.cpp`. Fully independent chain identity.

## 4. Testnet first

1. Configure `spectre.conf` with `testnet=1` (see `contrib/spectre.conf.example`).
2. Run two nodes; mine blocks on testnet.
3. Exercise transparent + shielded (`spec` / `tspec`) sends, RPC, and sync.
4. Confirm no connection to Zcash peers (different magic/ports).

## 5. Infrastructure

- [ ] At least 3 public seed nodes (DNS or fixed IP in `vSeeds`)
- [ ] Firewall: expose **8733** (P2P); keep **8732** RPC private or VPN-only
- [ ] Block explorer (e.g. forked explorer or indexer)
- [ ] Published binary builds + checksums
- [ ] Register BIP44 coin type 19117 if desired ([SLIP-0044](https://github.com/satoshilabs/slips/blob/master/slip-0044.md))

## 6. Mainnet launch

1. Announce network magic, ports, and activation (genesis or block 1).
2. Start seed nodes; publish `spectre.conf` snippet.
3. Mine block 1 (or new genesis if Option B).
4. Add checkpoints at stable heights as the chain matures.

## 7. Post-launch

- Monitor value pools and `getblockchaininfo`
- Add checkpoints to `checkpointData` periodically
- Set `consensus.nMinimumChainWork` after sufficient chain work accumulates

## What is already done in this repo

- Spectre branding (`spectred`, `spectre-cli`, `spectre.conf`)
- Network magic, ports, HRPs, BIP44 types
- Zcash EOL deprecation disabled
- Legacy Zcash checkpoints cleared
- Supply/halving/funding **rules** unchanged from Zcash

## What is not done (requires ops)

- Real multisig payout addresses
- DNS seeds / public peers
- New genesis (optional)
- Production binaries, explorer, exchange integrations
