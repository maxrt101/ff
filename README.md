# ff programming language

## About
ff is a general purpose scripting language with dynamic and gradual typing.  
It was developed to be easily used within C++ applications (which also means bindings for other languages should be fairly easy to implement).  


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

To make a debug build, use `-p debug` option, you can also provide a (comma separated) list of features with `--feature FEATURES`.  
Supported features: `LOG_STDOUT_ONLY`.  
Supported debug features: `MEM`, `REF`, `EVAL`, `TOKENS`, `SCOPES`, `GLOBALS`, `NOCATCH`.  

Build system keeps track of changed source files, and on subsequent builds will only recompile files that have changed. To force recompilation of everything, use `-f` flag.  

## Language

### 1. Values and operators
Every value is an object. `null` is used to mark an absence of value.  
Built-in types are `int`, `float`, `string`, `bool`, `function`, `dict`, `vector`.  
Numeric and string literals are supported, as well as `true` and `false` for booleans.  
Numeric literals can be decimal, hexadecimal or binary.  
String literals are enclosed in `"` and support escape sequences, such as `\n`, `\t`, `\r`, `\"`.  

Supported operators are: `+`, `-` (both unary and binary), `/`, `*`, `%`, `++`, `--`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `!`, `&&`, `||`, `=`, `:=`, and `as`.  

Operators work by calling an operator method on an object (except for `&&`, `||`, `=`, `:=` and `as`).  
Currently supported are `__add__`, `__sub__`, `__div__`, `__mul__`, `__mod__`, `__inc__`, `__dec__`, `__eq__`, `__neq__`, `__lt__`, `__gt__`, `__le__`, `__ge__`, `__not__` and `__neg__`.  

Also there are some special methods like `__bool__`, `__copy__` and `__assign__`.  

`__bool__` returns boolean for an object.  
For example a `string.__bool__` might return `false` if string is empty and `true` otherwise.  
`__bool__` is called on a value that is expected to be boolean, but is not. For example in `if`'s condition.  

`__copy__` returns a copy of the value.  

`__assign__` sets a value of a reference.  
Used for setting a value of a variable that is a reference for another variable.  
```
var x = 10;
var r = ref x;
r := 20; // same as r.__assign__(20) (effectively calls x.__assign__(20))
```

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

### 3. Type Inferring
As was mentioned above - types of some values can be inferred, for example:  
```
fn return100() -> {
  return 100;
}

var x = return100();
```
Even though we didn't add any annotations, the compiler will infer type of `return100` as `() -> int` and type of `x` as `int`.  

But consider next example:
```
fn returnUnion(i: int) -> {
  if (i == 0) {
    return 100;
  } else {
    return "abc";
  }
}
```

In this example the compiler will produce a `TypeMismatch` error, since there are 2 `return`s and each of them evaluates to a different type.  
There are 2 solutions to this problem:  
1. Add `any`:  
```
fn returnUnion(i: int): any -> {
...
```
2. Add union annotation:  
```
fn returnUnion(i: int): int | string -> {
...
```

Union annotations are considered better since they clearly show what to expect from a function.  

### 4. Variables
Variables are declared with `var` keyword followed by variable name.  
You can provide optional type annotation and initializer value.  
If type cannot be inferred, `any` will be assumed.  

```
var a;
var i = 100;
var s: str = "abc";
```

### 5. Functions
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

Inline functions (lambdas).
```
fn main() -> {
  var printer = fn(x) -> { print x; };
  var addone = fn(x) -> x + 1;

  printer(addone(10));
}

```

### 6. Conditionals
```
if (condition) {
  // then-body
} else {
  // else-body
}
```

### 7. Loops
For looping there are `loop`, `for` and `while` statements.  
`continue` and `break` are also supported.  

`loop` is an unconditioned infinite loop.   
```
var i = 0;
loop {
  ++i;
  if (i == 5) {
    break;
  }
}
```

`for` has 2 subsets - ordinary `for` and `foreach`:
```
for (var i = 0; i < 10; ++i) {
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
  ++i;
}
```

### 8. References
All values are allocated on the heap and passed around as references (pointers).  
By default values are copied, when the value is assigned to some variable.  
The values are copied using `__copy__` method.  
You can prevent the value from being copied by using `ref` keyword.  

Example:  
```
var i = 10;
print i;

var a = i;
print a;

++i;
print i;
print a;
```
Output:  
```
10
10
11
10
```

Using `ref`:  
```
var i = 10;
print i;

var a = ref i;
print a;

++i;
print i;
print a;
```

Output:  
```
10
10
11
11
```

References and functions.  
If you want to pass variable reference to a function, use `ref` on an argument.  

```
fn modify(x: int) -> {
  ++x;
}

fn main() -> {
  var i = 10;
  print i;

  modify(ref i);
  print i;
}
```

Output:  
```
10
11
```

Note that `x` holds a reference to `i`, if you try to assign another value to it the reference will get overwritten.  
```
fn modify(x: ref int) -> {
  ++x;
  x = 100;
}

fn main() -> {
  var i = 10;
  print i;

  modify(ref i);
  print i;
}
```

Output:  
```
10
11
```

To set a value to a reference, use `:=` operator:  
```
fn modify(x: ref int) -> {
  x := 100;
}

fn main() -> {
  var i = 10;
  print i;

  modify(ref i);
  print i;
}
```

Output:  
```
10
100
```

### 9. Annotations
You can annotate functions or variable declarations for convinience, or to inspect/alter their AST nodes.  
```
@print
fn test(x: ref int, y: int) -> {
  x := y;
}
```

`print` annotation will print the AST node that it annotates.  
In C++ code the implementation of `print` looks something like this:  
```C++
void ff::annotations::print(ast::Node* node) {
  printTree(node);
}
```

Annotated functions have their annotations stored in `__annotations__` field.  
```
print test.__annotations__;
```

Output:
```
{print}
```

### 10. Modules
Modules are practically namespaces, which you can load from other files.  

Example:  
`test.ff`:  
```
module test {
  var version = "1.0";

  fn printInfo() -> {
    print "This is module test v" + test.version;
  }
}
```

`main.ff`:  
```
import "test";

fn main() -> {
  test.printInfo();
  print test.version;
}
```

Modules can also be declared in source files and they can be nested:  
```
module mod {
  var x = 10;

  module nested {
    var y = 125;
  }

  fn function() -> {
    print "test";
  }
}

fn main() -> {
  mod.nested.y = 1;
  print mod.x;
  print mod.nested.y;
  mod.function();
}
```

Compiler resolves a module filename, by appending an import name to a set of folders (folder where source file is located and current working directory) and checking if the resulting path exists.  
Compiler will try to add `.ff` and `.ffmod` extensions to the import module name if the file with such name doesn't exist.  

Note: if imported file has other declarations apart from the imported module declaration, they will be ignored (except for imports).  
