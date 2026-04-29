# the mission is to create a compiler

## Lazy Lang Compiler – Implementation Roadmap

### Phase 1: Frontend (Lexer, Parser, AST)
- [ ] 1.1 Implement Lexer (scan all tokens: `let`, `mut`, `match`, etc.)
- [ ] 1.2 Define AST node types (Expr, Stmt, Pattern, Type)
- [ ] 1.3 Build Recursive Descent Parser (handle struct/block disambiguation, expression‑oriented `if`/`match`, `lazy let`)
- [ ] 1.4 Write Parser tests (nested `match`, `if let`, range patterns)

### Phase 2: Middle End (Type System & Semantic Analysis)
- [ ] 2.1 Implement Symbol Table with scoping and mutability tracking
- [ ] 2.2 Create Type Infrastructure (distinct `i8`…`i128`, `u8`…`u128`, `f32`, `f64`, `bool`, `str`)
- [ ] 2.3 Add Bidirectional Type Inference (`let x = 5` → `i32`, function return type checking)
- [ ] 2.4 Semantic Checks:
  - [ ] Mutability enforcement (no mutation without `mut`)
  - [ ] Pattern exhaustiveness for `match`
  - [ ] Unused variable / unreachable code detection
- [ ] 2.5 Produce Annotated Typed AST (discriminant offsets for enums, types on every node)

### Phase 3: Backend (IR, Optimisation & Codegen)
- [ ] 3.1 Build SSA IR Generator (basic blocks, phi nodes for match/if, loop lowering)
- [ ] 3.2 Implement Control Flow for Early Return (`ret`) and Implicit Return (last expression)
- [ ] 3.3 Optimisation Passes:
  - [ ] Constant folding & dead code elimination
  - [ ] Pattern‑specific optimisations (range checks → jump tables, merge overlapping arms)
  - [ ] Function inlining
- [ ] 3.4 LLVM Codegen (map Lazy Lang types to LLVM, emit debug info)
- [ ] 3.5 WebAssembly Codegen (via LLVM, provide `lazy_rt` stub)

### Phase 4: Standard Library & Runtime
- [ ] 4.1 Implement Runtime Library (`lazy_rt.lazy`) – heap allocation, atomic reference counting for `str`
- [ ] 4.2 Add `print` / `println` Intrinsics (format specifiers for all primitive types)
- [ ] 4.3 Define `str` Layout & CoW behaviour (immortal literals, RC‑managed buffers)
- [ ] 4.4 Write Integration Tests (all control flow + I/O, check memory leaks)


# Information about the language

## Variables
lazy `
    let variable:type = value; // immutable value
    let mut variable:type = value; // mutable value
    
    // you can also do this

    let variable = value; // immutable value
    let mut variable = value; // mutable value
`

### Variables types
- bool  1 / 0 true / false

- i8    signed   8 bit intiger
- i16   signed  16 bit intiger
- i32   signed  32 bit intiger
- i64   signed  64 bit intiger
- i128  signed 128 bit intiger

- u8    unsigned   8 bit intiger
- u16   unsigned  16 bit intiger
- u32   unsigned  32 bit intiger
- u64   unsigned  64 bit intiger
- u128  unsigned 128 bit intiger

- f32   float with 32 bit intiger
- f64   float with 64 bit intiger

- strings str

## Control Flow

### you have all the normal operators

- or  == ||
- and == &&
- not == !

lazy `

if x > 0 {
    println("positive")
} elif x < 0 {
    println("negative")
} else {
    println("zero")
}

while alive {
    tick()
}

for i in 0..10 {
    print(i," \n")
}

for item in items {
    process(item)
}


// if also an expration
if let p = false {
    println("show you debut state")
}

// same for while
while let bool = treu {
    run()
}

`

## Functions
    simple functions are exprations by default in this language so you don't need to use the ret (return) keyword
    you can use it for early exit

lazy `

//function_name(args) -> return_type {
//    code
//}

// this is a void function
do_nothing(){

}

// example
add(a: i32, b:i32) -> i32 {
    a + b
}

greet(name: str) {
    println("hello, {name}")
}

early_exit(x: i32) -> i32 {
    ret if x < 0 {0}
    x * 2
}
`

## we use match insted of switch
lazy `

let kind = match number {
    0    => "zero"
    1..9 => "single digit"
    _    => "many"
}

// you can also do this kind of stuf

match number {
    1 => {
        // code ...
        ret 1
    }

    // this is also true
    0 => {
        // code ...
    } ret 0

    _ => -1
}

`

## structs and enums
lazy `

struct Point {
    x: int,
    y: int,
}

let p : Point = { 10 , 20}

// or
let p = { .x = 10 , .y = 20}


enum Color { Red, Green, Blue }

enum Result {
    Ok(value: int),
    Err(msg: str),
}

match res {
    Result.Ok(v) => println("got: {v}")
    Result.Err(m) => println("err: {m}")
}
`
