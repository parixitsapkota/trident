# Trident language Spec

# Lexical

## Lexical Rules

- DECIMAL_INTS : `[0-9][0-9]*U?`
- OCTAL_INTS : `0_[0-7]+U?`
- HEX_INTS : `0x[0-9A-F]+U?`
- DECIMAL_FLOATS : `[0-9]*(.[0-9]*(e[-+]?[0-9]+)?)`
- OCTAL_FLOATS : `0_[0-7]+(.[0-7]*)?p[-+]?[0-7]+`
- HEX_FLOATS : `0x[0-9A-F]+(.[0-9A-F]*)?p[-+]?[0-9A-F]+`
- IDENTIFIERS : `[_a-zA-Z][_a-zA-Z0-9]*`
- UTF-8_IDENTIFIERS : `@"[^"\n]*"`
- STRING : `"[^"\n]*"`
- CHARACTER : `'[^'\n]*'`
- DIRECTIVES : `@[_a-zA-Z][_a-zA-Z0-9]*`

## comments

```trident
// hello, world

fn main() {
  // I'm feeling lucky today
  lucky_number := 7;
}
```

## Reserved keywords

- Qualifiers  
  `inline` `extern` `static` `volatile`  
  `register` `pub` `var` `const`

- Control flow  
  `if` `else` `do` `while` `for` `break`  
  `continue` `switch` `case` `default`

- Functions  
  `fn` `return`

- Types  
  `bool` `false` `true`  
  `type` `void` `f32` `f64` `f128`  
  `i8` `i16` `i32` `i64` `i128` `isize`  
  `u8` `u16` `u32` `u64` `u128` `usize`  
  `struct` `union` `enum` `error`

- Compiler directives  
  `@import` `@sizeof` `@typeof` `@define`

# Scalar Types

A scalar type represents a atomic value.

## Integer Types

| Signed | Unsigned | Bit Lenth | Byte Lenth |
| ------ | -------- | --------- | ---------- |
| u8     | i8       | 8 bits    | 1 bytes    |
| u16    | i16      | 16 bits   | 2 bytes    |
| u32    | i32      | 32 bits   | 4 bytes    |
| u64    | i64      | 64 bits   | 8 bytes    |
| u128   | i128     | 128 bits  | 16 bytes   |
| usize  | isize    | Arch-word | Arch-word  |

## Float Types (IEEE 754)

| float | Bit Lenth | Byte Lenth |
| ----- | --------- | ---------- |
| f32   | 32 bits   | 4 bytes    |
| f64   | 64 bits   | 8 bytes    |
| f128  | 128 bits  | 16 bytes   |

## Numeric

| Number literals | Example    |
| --------------- | ---------- |
| Decimal int     | 99         |
| Hex int         | 0xff       |
| Octal int       | 0_77       |
| Decimal float   | 12.984e-2  |
| Hex float       | 0xF5.2Ep-2 |

> _Integer Overflow : Example, in case of a `u8`, the value 256 becomes 0, the value 257 becomes 1, and so forth._

## Boolean

Boolean type has two possible values: `true` and `false`. Booleans are one byte in size.

# Compound Types

> _TODO :_ Define compound types like arrays and structs

# Operator Precedence

| P.  | Operator   | Description                                    | Associativity |
| --- | ---------- | ---------------------------------------------- | ------------- |
| 1   | ()         | Parentheses                                    | Left-to-right |
| 1   | ++ --      | Suffix increment / decrement                   | Left-to-right |
| 1   | func()     | Function call                                  | Left-to-right |
| 1   | []         | Array subscripting                             | Left-to-right |
| 1   | .          | Member access                                  | Left-to-right |
| 1   | @u32toi32  | Expression compiler directives                 | Left-to-right |
| 2   | ++ --      | Prefix increment / decrement                   | Right-to-left |
| 2   | + -        | Unary plus and minus                           | Right-to-left |
| 2   | ! ~        | Logical NOT, bitwise NOT                       | Right-to-left |
| 2   | \*         | Dereference                                    | Right-to-left |
| 2   | &          | Reference                                      | Right-to-left |
| 3   | \* / %     | Multiplication, division, and remainder        | Left-to-right |
| 4   | + -        | Addition and subtraction                       | Left-to-right |
| 5   | << >>      | Bitwise left shift and right shift             | Left-to-right |
| 6   | < <=       | less than, less than or equal to               | Left-to-right |
| 6   | > >=       | greater than, greater than or equal to         | Left-to-right |
| 7   | == !=      | Relational equal to, not equal to              | Left-to-right |
| 8   | &          | Bitwise AND                                    | Left-to-right |
| 9   | ^          | Bitwise XOR (exclusive OR)                     | Left-to-right |
| 10  | \|         | Bitwise OR (inclusive OR)                      | Left-to-right |
| 11  | &&         | Logical AND                                    | Left-to-right |
| 12  | \|\|       | Logical OR                                     | Left-to-right |
| 13  | .. ..< ..> | Range                                          | Left-to-right |
| 14  | =          | assignment                                     | Right-to-left |
| 14  | += -=      | Assignment by sum and difference               | Right-to-left |
| 14  | \*= /= %=  | Assignment by product, division, and mod       | Right-to-left |
| 14  | <<= >>=    | Assignment by bitshift left and bitshift right | Right-to-left |
| 14  | &= ^=      | Assignment by bitwise AND, XOR, and OR         | Right-to-left |
| 15  | ,          | Comma                                          | Left-to-right |

## Identifiers

UTF-8 string Identifiers :

- using the @"" syntax Identifiers can contain any UTF-8 symbol, But don't escape sequences and must remain on a single line.

```trident
var @"中文" : []u8;
@"中文" = "china";
```
