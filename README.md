# `ff` programming language

## About
`ff` is a general purpose programming language with dynamic and gradual typing.
It was developed to be easily paired with C++ applications.

## Building
### Prerequisites:
  - Linux or MacOS environment
  - `C++` compiler that supports `c++17` (tested with `apple clang 11.0.3`)
  - `python3` (tested with `3.10`)

### Steps:
 - Clone the repo
 - `cd ff`
 - `git submodule update --init --recursive`
 - `./make.py`

Executable will appear in `target/release/bin`, headers and libraries in `target/release/include` and `target/release/lib` respectively.  

For more verbose output use `-v`  

To make a debug build, use `-p debug` option, you can also provide a (comma separated) list of debug fatures with `--feature FEATURES`.  
Supported debug features are `MEM`, `REF`, `EVAL`, `TOKEN`, `PARSER`, `NOCATCH`.  

Build system keeps track of changed source files, and on subsequent builds will only recompile changed files. To force recompilation of everything, use `-f` flag.  

## Language

### 1. Values and operators
Every value is an object. `null` is used to mark an absence of value.  
Built-in types are `int`, `float`, `string`, `bool`.  
Numeric and string literals are supported, as well as `true` and `false` for booleans.  
Numeric literals can be decimal, hexadecimal or binary.  
String literals support escape sequences, such as `\n`, `\t`, `\r`, `\"`.  

### 2. Variables
Variables are declared with `var` keyword followed by variable name. You can provide optional type annotation and initializer value. If type cannot be inferred, `any` will be assumed.

```
var a;
var i = 100;
var s: str = "abc";
```

### 3. Functions
Functions are declared using `fn` keyword followed by function name and parameter list. Every parameter can have optional type annotation as well as the function itself. For any type that cannot be inferred, `any` will be assumed. After parameter list `->` must be placed. Function body can be either a block or an expression. 

```
fn main() -> {}

fn add(a: int, b: int): int -> a + b;
```

### 4. Control flow

```
if (condition) {
  // then-body
} else {
  // else-body
}
```

### 5. Loops
For looping there is `loop`, `for` and `while`. `continue` and `break` are also supported.

`loop` is an unconditioned infinite loop.   
```
var i = 0;
loop {
  i = i + 1;
  if (i == 5) {
    break;
  }
}
```
`for` and `while` are currently under development.  
