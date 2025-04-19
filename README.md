# Mini Pascal Compiler

A simple compiler for a subset of the Pascal programming language. This project includes a lexical analyzer, syntax analyzer, semantic checker, and code emitter that generates pseudo-assembly for a stack-based machine.

## Features

- ✅ Lexical analysis (tokenizes Pascal keywords, identifiers, numbers, operators, etc.)
- ✅ Syntax analysis using a recursive descent parser
- ✅ Semantic analysis for type checking and symbol table management
- ✅ Code generation into a stack-based intermediate representation
- ✅ Simple error handling for syntax errors
- 🧪 Debug-friendly output to track compilation stages

## Supported Pascal Subset

The compiler currently supports:

- `program` declaration
- `var` declarations of type `int`
- Basic statements: assignment (`:=`), `if-then-endif`, `writeln`, `readln`
- Arithmetic expressions with `+`, `*`
- Relational expressions: `=`, `<`, `>`, `<=`, `>=`, `<>`
