# SUBSYSTEM : Parser

Processes the flat stream of tokens produced by the lexer and builds a hierarchical Abstract Syntax Tree (AST) conforming to the language grammar. The parse does lenient parsing in fabour of better and informed error messages.

- **Lenient Parsing**: The parser recognizes what a construct is syntactically (e.g. a return stmt), even if it's in the wrong place, the parser parses it correctly without complaning.

## Files

- **`parser.c` / `parser.h`**: Recursive descent parser engine.
- **`stmt.c`**: Parsing logic for statements, control flow structures, blocks, and declarations.
- **`expression.c`**: Parsing logic for expressions, strict operator precedence, and associativity.
- **`process.c` / `process.h`**: lookahead handling and other parsing utility funcrions.

## Dependencies

- **`core diags subsystem`**
- **`syntax token subsystem`**
- **`syntax tree subsystem`**
