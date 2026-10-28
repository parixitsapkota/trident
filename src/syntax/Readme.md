# MODULE : Syntax

The `syntax` module groups all subsystems responsible for transforming the raw source code text into a structured, hierarchical Abstract Syntax Tree (AST). As a container module without a direct public header, its components expose their own specific public interfaces.

## Subsystems & Modules

- **[`token`](token/Readme.md)**: Defines token structures, types, keywords, and intrinsics.
- **[`lexer`](lexer/Readme.md)**: Scans the raw character stream and breaks it down into discrete tokens.
- **[`parser`](parser/Readme.md)**: Processes the token stream to construct the syntax tree according to the grammar rules.
- **[`ast`](ast/Readme.md)**: Manages Abstract Syntax Tree (AST) node structures, printing, and formatting.

---

# Notes

> [!NOTE]
> TODO: Make it so that tokens and the ast out live the lexer and parser respectively.
