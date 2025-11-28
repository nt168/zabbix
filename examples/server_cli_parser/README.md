# Zabbix server command-line parser example

This small project extracts the Zabbix server `main()` option handling loop into a
standalone program. It demonstrates how the server validates options such as
`-c/--config`, `-R/--runtime-control`, `-T/--test-config`, `-f/--foreground`,
`-h/--help`, and `-V/--version`, along with duplicate and mutual-exclusion checks.

## Build

```sh
make    # builds ./server_cli_parser
```

The Makefile uses the default C compiler (`cc`) and enables strict warnings
(`-Wall -Wextra -Wpedantic -Werror`). A `make clean` target removes the produced
binary so the directory stays portable in version control.

## Usage examples

```sh
./server_cli_parser -c /etc/zabbix/zabbix_server.conf -f
./server_cli_parser -T
./server_cli_parser -R config_cache_reload
```

Use `-h` for the help text and `-V` for the version banner.
