# Building Trident

> [!NOTE]
> `gperf` needs to be a globally available command.

> [!NOTE]
> Trident explicitly uses the `-MJ` flag from `clang`. You may experience friction using a different compiler.

> [!NOTE]
> On windows system, it is recommended to use WSL for a smoother experience.

## Linux / Unix Systems

### Dependencies

- `make`
- `clang`
- `gperf`
- `find`

### Build Command

To build the compiler in release mode, run:

```bash
make MODE=release
```
