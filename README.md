Spectre
=======

Privacy-focused cryptocurrency node, forked from [Zcash](https://github.com/zcash/zcash).

| | |
|---|---|
| **Ticker** | SPEC |
| **Daemon** | `spectred` |
| **CLI** | `spectre-cli` |
| **Config** | `spectre.conf` |
| **Data dir** | `~/.spectre` (Unix) · `%APPDATA%\Spectre` (Windows) |

## Status

Pre-release (`0.1.0`). Chain identity, ports, and address prefixes are configured for an independent Spectre network. Genesis still matches Zcash until block 1 is mined (or a new genesis is mined). Founders/funding addresses are **placeholders** — replace before mainnet.

See [doc/CHAIN.md](doc/CHAIN.md) and [doc/LAUNCH.md](doc/LAUNCH.md).

## Build

```bash
./zcutil/fetch-params.sh
./zcutil/build.sh -j$(nproc)
```

Details: [INSTALL](INSTALL)

## Documentation

| Doc | Purpose |
|-----|---------|
| [INSTALL](INSTALL) | Build and first run |
| [SECURITY.md](SECURITY.md) | Report vulnerabilities, operational safety |
| [COPYING](COPYING) | MIT license (required) |
| [doc/developer-notes.md](doc/developer-notes.md) | Developer conventions |
| [doc/unit-tests.md](doc/unit-tests.md) | Tests |
| [contrib/docker/](contrib/docker/) | Legacy upstream Zcash Docker (not Spectre builds) |
| [doc/ARCHITECTURE.md](doc/ARCHITECTURE.md) | Codebase structure and runtime diagram |
| [doc/CHAIN.md](doc/CHAIN.md) | Network ports, HRPs, genesis, checkpoints |
| [doc/LAUNCH.md](doc/LAUNCH.md) | Pre-mainnet launch checklist |
| [contrib/spectre.conf.example](contrib/spectre.conf.example) | Example node config |

## License

MIT — see [COPYING](COPYING). Spectre modifications © 2026 sbf-developer. Upstream © Bitcoin Core and Zcash developers.
