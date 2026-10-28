# SUBSYSTEM : Lexer

Responsible for scanning the raw source text buffer and transforming it into a discrete, ordered stream of categorized tokens.

## Files

- **`lexer.c` / `lexer.h`**: Core lexer loop, state management.
- **`digit.c`**: Specialized parsing routines for integer, floating-point, and custom numeric literals.
- **`escape.c`**: Handles string and character escape sequence translation.
- **`keywords.def` / `keywords.h`**: Keyword lookup tables, string hashing/mapping, and reserved identifier checks.
- **`intrinsics.def` / `intrinsics.h`**: Definitions for built-in language intrinsics and compiler-recognized hooks.
- **`process.c` / `process.h`**: source advancement, and error handling for invalid tokens.

## Dependencies

- **`core source subsysten`**
- **`core diags subsysten`**
- **`syntax token subsystem`**
