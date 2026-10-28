# Code Guide

The codebase follows a component-based design where each directory represents a subsystem.

- **Public Subsystems:** Contain a header file matching the directory name (e.g., `type.h` inside the `type/` directory), which exposes its official public API.
- **Modules:** Directories that do not contain a matching header file act as container modules grouping multiple related subsystems.

## Inclusion & Visibility Rules

1. **Public Headers Only:** Only public header definition files may be included by other subsystems.
2. **Internal Subsystems:** If a subsystem does not provide a public header, it is considered internal to its parent module and must not be used or included outside of that module boundary.

---

# Trident Source Root

## Subsystem Dependencies

- **All Subsystems**: As the top-level driver, `main.c` orchestrates and relies on all primary compiler subsystems to execute the compilation pipeline.

## Subsystems & Modules

- **[`core`](core/Readme.md)**: Foundational infrastructure, memory management (arenas), diagnostics, and source buffers.
- **[`syntax`](syntax/Readme.md)**: Lexical and syntactic pipeline (lexer, parser, AST structures).
- **[`type`](type/Readme.md)**: Type system representations and helper utilities.

---

# Notes

Any notes regarding the code, architectural decisions, or implementation details excluding inline `TODO` comments—must be documented directly in the respective module's or subsystem's `Readme.md` or any other `*.md` file within the module's or subsystem's rather than scattered across source files.
