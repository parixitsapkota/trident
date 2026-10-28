# SUBSYSTEM : AST

Defines the memory structures, node types, and debugging utilities for the Abstract Syntax Tree (AST).

## Files

- **`ast.c` / `ast.h`**: Node type definitions, structure layouts, and constructors for expressions, statements, and declarations.
- **`print.c`**: Debug utility for dumping human-readable, indented representations of the AST structure.
- **`format.c`**: Code formatting / AST-to-text reconstruction utilities.

## Dependencies

- **`core mem subsystem`**
- **`syntax token subsystem`**
