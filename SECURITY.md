# Security

Spectre inherits the Zcash/Bitcoin node codebase. Treat wallet keys, RPC credentials, and proving parameters as sensitive.

## Reporting vulnerabilities

Use [GitHub Security Advisories](https://github.com/sbf-developer/spectre-crypto/security/advisories/new) for this repository.

Please include:

- A clear description of the issue
- Steps to reproduce (or a proof-of-concept)
- Impact (consensus, privacy, wallet funds, remote DoS, etc.)

Do **not** open public issues for exploitable security bugs.

## Severity guide

**Critical:** consensus breaks, unintended coin creation, remote loss of funds, shielded privacy breaks.

**High:** network splits, sustained remote DoS, other remote fund-loss paths.

**Moderate / Low:** local wallet issues, bounded resource use, non-exploitable crashes.

## Operational safety

- Run `spectred` on a dedicated machine or VPS; firewall RPC (default port 8232).
- Use `-rpcuser` / `-rpcpassword` or `-rpcauth`; never expose RPC to the public internet without TLS and strict access control.
- Back up wallet seed / `wallet.dat` offline before mainnet use.
- Download proving params only via `./zcutil/fetch-params.sh` or official release artifacts.
- This project is **early-stage** — not audited for production use with real funds.

## License

Security policy text for this file: MIT, same as [COPYING](COPYING).
