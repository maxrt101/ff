# ff programming language

## About
ff is a general purpose programming language with dynamic and gradual typing.  
It was developed to be easily used within C++ applications (which also means binding for other languages should be fairly easy to implement).  

## Building
### Prerequisites:
  - Linux or MacOS environment
  - `C++` compiler that supports `c++17` (tested with `apple clang 11.0.3` and `gcc 10.2.0`)
  - `python3` (tested with `3.10.0`)

### Steps:
 - Clone the repo
 - `cd ff`
 - `git submodule update --init --recursive`
 - `./make.py`

Executable will appear in `target/release/bin`, headers and libraries in `target/release/include` and `target/release/lib` respectively.  

For more verbose output use `-v`  

To make a debug build, use `-p debug` option, you can also provide a (comma separated) list of debug fatures with `--feature FEATURES`.  
Supported debug features are `MEM`, `REF`, `EVAL`, `TOKEN`, `SCOPES`, `GLOBALS`, `NOCATCH`.  

Build system keeps track of changed source files, and on subsequent builds will only recompile files that have changed. To force recompilation of everything, use `-f` flag.  

## Language

### 1. Values and operators
Every value is an object. `null` is used to mark an absence of value.  
Built-in types are `int`, `float`, `string`, `bool`.  
Numeric and string literals are supported, as well as `true` and `false` for booleans.  
Numeric literals can be decimal, hexadecimal or binary.  
String literals are enclosed in `"` and support escape sequences, such as `\n`, `\t`, `\r`, `\"`.  

Supported operators are: `+`, `-` (both unary and binary), `/`, `*`, `%`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `!`, and `as`.  

Operators work by calling an operator method on an object.  
Currently supported are `__add__`, `__sub__`, `__div__`, `__mul__`, `__mod__`, `__eq__`, `__neq__`, `__lt__`, `__gt__`, `__le__`, `__ge__`, `__not__` and `__neg__`.  

Also there is `__bool__` that returns boolean for an object.  
For example a `string.__bool__` might return `false` if string is empty and `true` otherwise.  
`__bool__` is called on a value that is expected to be boolean, but is not. For example in `if`'s condition.  

### 2. Types
Every value has a type.  
Type checking is performed in compile-time and in runtime.  
You can provide a type to a variable or function by annotating it with `identifier: type` syntax.  
Types of some values can be inferred, if type cannot be inferred, `any` is used.  
`any` is basically a wildcard, variables of such type can hold anything.  

Type casting. To cast a value to some other type, `as` operator is used:
```
var a = 10 as string;
var b = 5 as float;
```
Casting works by calling a special method that handles the cast.  
For `x as string`, where `x` is of type `T`, `T` has to implement `__as_string__`, for `x as int`, `T` has to implement `__as_int__`.  

### 3. Variables
Variables are declared with `var` keyword followed by variable name.  
You can provide optional type annotation and initializer value.  
If type cannot be inferred, `any` will be assumed.  

```
var a;
var i = 100;
var s: str = "abc";
```

### 4. Functions
Functions are declared using `fn` keyword followed by function name and parameter list.  
Every parameter can have optional type annotation as well as the function itself.  
For any type that cannot be inferred, `any` will be assumed.  
After parameter list `->` must be placed.  
Function body can be either a block or an expression.  

```
fn add(a: int, b: int): int -> a + b;

fn main() -> {
  var res = add(10, 20);
}
```

### 5. Conditionals
```
if (condition) {
  // then-body
} else {
  // else-body
}
```

### 6. Loops
For looping there are `loop`, `for` and `while` statements.  
`continue` and `break` are also supported.  

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

`for` has 2 subsets - ordinary `for` and `foreach`:
```
for (var i = 0; i < 10; i = i + 1) {
  print i;
}

for x in iterable {
  print x;
}
```

`while`:
```
var i = 0;
while (i < 10) {
  print i;
  i = i + 1;
}
```
