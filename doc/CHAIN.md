# Spectre chain identity

Spectre is a Zcash-protocol fork with its own network identity. Supply rules (subsidy, halving, founders reward schedule, ZIP 214 funding streams) match upstream Zcash consensus; only network identity and payout **addresses** differ.

## Networks

| | Mainnet | Testnet | Regtest |
|---|---|---|---|
| **Ticker** | SPEC | TSPEC | RSPEC |
| **P2P port** | 8733 | 18733 | 18344 |
| **RPC port** | 8732 | 18732 | 18232 |
| **P2P magic** | `SPEC` | `TSPC` | (regtest default) |
| **BIP44 coin type** | 19117 | 19118 | 1 |
| **Data dir** | `.spectre` | `.spectre-testnet` | `regtest` |

## Address prefixes (mainnet)

| Type | Prefix | Example |
|------|--------|---------|
| Transparent P2PKH | `s1` | `s1...` |
| Transparent P2SH | `s3` | `s3...` |
| Sapling shielded | `spec` | `spec1...` |
| TEX (Orchard) | `stex` | `stex1...` |
| Sprout (legacy) | `sc` | `sc...` |

Testnet shielded HRP: `tspec` (and related `tspecviews`, `tspecivks`, etc.).

## Genesis

Mainnet and testnet currently use the **Zcash genesis block** (Equihash `(200,9)`). That gives a known starting point but means Spectre shares genesis hash with Zcash until block 1 is mined on the Spectre network.

To launch a fully independent chain, mine a new genesis (new timestamp/nonce/solution) and update `src/chainparams.cpp` asserts accordingly.

## Checkpoints

Legacy Zcash checkpoints and supply/value-pool checkpoints are cleared. Only height `0` (genesis) is hardcoded. Add checkpoints as the chain grows.

## Founders reward & funding addresses

`src/chainparams.cpp` contains **deterministic placeholder** P2SH addresses (generated via `contrib/devtools/gen_spectre_addresses.py`). They decode correctly at node startup but are not real multisig wallets.

**Before mainnet launch:** replace founders reward and ZIP 214 addresses with real 2-of-3 multisig addresses controlled by the Spectre project, using `spectred-wallet-tool` after building the node.

## DNS seeds

Seed lists are empty. Add DNS seeds to `vSeeds` in `chainparams.cpp` before public launch, or publish fixed seed IPs.

## Deprecation

Zcash `zcashd` EOL auto-shutdown is disabled. `EnforceNodeDeprecation()` is a no-op; `DEPRECATION_HEIGHT` is set far in the future.

## Params

Run `./zcutil/fetch-params.sh` once per machine. Params install to `~/.spectre-params` (or `%APPDATA%\SpectreParams` on Windows).
