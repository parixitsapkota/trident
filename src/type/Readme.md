# SUBSYSTEM : Type

Manages type representation. Note that actual type checking is handled by the analyze; this module serves primarily as a core helper for creating, representing, and querying types.

# Type

## Files

- **`type.c` / `type.h`**: Core type data structures, layout definitions, and helper utilities for constructing and managing types across compilation phases.

### Subsystem Dependencies

This subsystem depends on:

- `core mem subsystem`
- `syntax token subsystem`
