# FIT VUT — ISA — LDAP server

## Project structure
- `doc/` — documentation
- `src/` — source code
- `data/` — input data
- `tests/` — tests

## Usage
```
isa-ldapserver {-p <port>} -f <file>
```
- `-p <port>` — port to listen on (optional, default `389`)
- `-f <file>` — input file with data in CSV format