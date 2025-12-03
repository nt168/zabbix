# IPC service test helpers

This directory is built by the top-level Autotools project. To generate the
`Makefile` that lets you build `mprocess` and `sprocess`, run the usual
bootstrap and configure steps from the repository root:

```
./bootstrap.sh && ./configure
```

After configuration, invoking `make` from the repository root will build the
helper binaries as part of the `tests/libs` tree. Running `make` directly in
this directory before configuring will fail because the generated `Makefile`
does not yet exist.
