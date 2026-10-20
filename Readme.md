> [!WARNING]
> Trident is currently under development and is subject to change.  
> Documentation may not always represent the current state of the project and compiler internals.

> [!NOTE]
> Build instructions are in [docs/build.md](docs/build.md).
> If your system or setup is not available, you can become a contributor and create a PR to add it.  
> Want to contribute? Email me ;) The project is currently closed to contributors and collaborators only.

<div align="center">
  <p>
    <img alt="License" src="https://img.shields.io/github/license/parixitsapkota/trident?style=for-the-badge&color=00b894">
    <img alt="top language" src="https://img.shields.io/github/languages/top/parixitsapkota/trident?style=for-the-badge&color=0984e3">
    <img alt="last commit" src="https://img.shields.io/github/last-commit/parixitsapkota/trident?style=for-the-badge&color=e17055">
    <img alt="Repo stars" src="https://img.shields.io/github/stars/parixitsapkota/parixitsapkota?style=for-the-badge&color=fdcb6e">
  </p>
</div>

---

**Trident** is an _imperative_, _general-purpose_, _statically typed_ programming language designed for **software systems**.

---

## Philosophy

I believe that code should be as organic as the nature that inspires it, and this should be reflected in code while reading it. That is why I chose C as an inspiration: despite having hidden and unexpected behaviors, C reads clearly regarding the intent of the programmer.

Trident is designed to be expressive with its syntax and natural to read while having ergonomic features. Trident is created from a deep love of C, so it is not a C alternative; rather, I envision Trident to be used **alongside** C, not in place of C.

## Design

### Paradigm

Trident is an **Object-Based Programming (OBP)** language, not to be confused with Object-Oriented Programming (OOP).

Being a component-based language, Trident does not support classic OOP features like classes, namespaces, or inheritance. Instead, it takes a different path utilizing modules and visibility qualifiers for name resolution and interfaces.

### Memory & Safety

Trident is designed with memory management in mind, providing explicit handling for nullability and optionals. The standard library explicitly avoids hidden allocations; memory is entirely handled by the user, making allocation explicit so that a programmer is never caught unaware by hidden allocations that require freeing.

### Core Architectural Advantages

- **No Hidden Costs**: Without virtual method tables, dynamic dispatch, or inheritance casting, every function call is statically known at compile time.
- **Cache Friendly**: Structs with plain data lay out linearly in memory, maximizing CPU cache utilization.
- **No "The Fragile Base Class" Problem**: Programmers never have to worry about changing a base struct breaking a deep inheritance tree because there are no inheritance hierarchies.

Trident shares the exact same architectural lineage as C.

## Features

- Explicit user-system level recoverable/fatal faults.
- Errors and functions as first-class types.
- Strict explicitness throughout the syntax.
- Support for meta-types (compile-time-only `type` parameters like `func sizeof(type T)` for layout introspection, avoiding runtime bloat).
- No hidden control flows.
- Allocation and allocator-aware design.

## Tooling

Trident tooling is very minimal and only has things that are openionated.

### LSP - NO-PLANS!

The compiler is enough to output structured error diagnostics.

### Build system

Everything lives in the config file (build.conf). It handles nested projects explicitly and deterministically without magic package managers or dynamic dependency resolution.

### Formator

Zero configuration flags. There is only one correct way to format Trident code. If it doesn't match the canonical form, the toolchain formats it to the canonical form.

### Documentation

Instead of external manual header files and documentation generation, the compiler itself parses dedicated doc-comments and automatically generates clean interface definition files and documentation artifacts straight from the source.
