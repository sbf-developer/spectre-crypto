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

Early fork — chain parameters and genesis are not finalized. **Not production-ready.**

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
| [contrib/docker/](contrib/docker/) | Docker setup |
| [contrib/seeds/](contrib/seeds/) | Seed-node generator (for new networks) |

## License

MIT — see [COPYING](COPYING). Spectre modifications © 2026 sbf-developer. Upstream © Bitcoin Core and Zcash developers.
