# Spectre chain parameters

Reference for network identity configured in `src/chainparams.cpp` and `src/chainparamsbase.cpp`. Consensus rules (supply, halving, ZIP activations) follow the Zcash 6.x lineage unless noted.

## Status

Pre-release **0.1.0**. Mainnet uses **Option A** from [LAUNCH.md](LAUNCH.md): the genesis block still matches Zcash; Spectre diverges once block 1 is mined on the Spectre network. Founders and lockbox addresses in `chainparams.cpp` are **placeholders** — replace before production mainnet.

## Binaries and paths

| Item | Value |
|------|-------|
| Daemon | `spectred` |
| CLI | `spectre-cli` |
| Config file | `spectre.conf` |
| Data directory | `~/.spectre` (Unix), `%APPDATA%\Spectre` (Windows) |
| Proving params | `~/.spectre-params` / `SpectreParams` |
| Currency code | `SPEC` |

## Mainnet

| Parameter | Value |
|-----------|-------|
| P2P magic | `SPEC` (`0x53 0x70 0x45 0x43`) |
| P2P port | **8733** |
| RPC port | **8732** |
| BIP44 coin type | **19117** (unregistered) |
| Transparent pubkey addresses | `s1…` prefix |
| Transparent script addresses | `s3…` prefix |
| Sapling payment HRP | `spec` |
| Tex (ZIP 320) HRP | `stex` |

## Testnet

| Parameter | Value |
|-----------|-------|
| P2P magic | `TSPC` |
| P2P port | **18733** |
| RPC port | **18732** |
| BIP44 coin type | **19118** |
| Sapling HRP | `tspec` |
| Tex HRP | `tstex` |
| Data subdir | `spectre-testnet` under the data directory |

## Regtest

| Parameter | Value |
|-----------|-------|
| P2P port | **18344** |
| RPC port | **18232** |
| Sapling HRP | `rspec` |

## Isolation from Zcash

Spectre nodes must **not** peer with Zcash: magic bytes, ports, and address HRPs differ. Wallets must use Spectre coin type and HRPs so funds are not sent to the wrong chain.

## Genesis

Current genesis is the Zcash mainnet genesis (Equihash 200,9). For a fully independent chain identity, mine a new genesis and update `CreateGenesisBlock` in `chainparams.cpp` (Option B in [LAUNCH.md](LAUNCH.md)).

## Checkpoints and minimum chain work

Legacy Zcash checkpoints are cleared. Add checkpoints and raise `nMinimumChainWork` as the Spectre chain matures.

## Related files

- `contrib/spectre.conf.example` — example node configuration
- `contrib/devtools/gen_spectre_addresses.py` — dev address helpers (not for production keys)
- [LAUNCH.md](LAUNCH.md) — operational launch checklist
