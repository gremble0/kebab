# Kebab
## Introduction
Kebab is a strongly typed primarily functional programming language that is currently under development. Here is how it looks:

```clj
; `;` for comments
; Basic syntax for the language looks like this. The parser ignores any whitespace
; and newlines so the following are both valid definitions.
def this-is-fine
= int( 2
             +
                       3   )
def but-probably-better-like-this = int(2 + 3)

; Some basic constructors
def i = int(69 + 420)
def l1 = list((int) => [1, 2, 3])
def l2 = list((string) => ["123", "gg", "hhh"])
def add-one = fn((a : int) => int(a + 1))
def b1 = bool(1 == -2)
def b2 = bool(1 ~= -2)

; Nested statements inside constructors. The constructor returns the last expression
def nested-example = int(
  def nested = int(5)
  2 + nested
)

; Functions are first class citizens and can be made local to some constructor.
; Here `local-fn` is only visible from inside the int constructor for `nested-example2`
def nested-example2 = int(
  def local-fn = fn(() => int(42))
  local-fn()
)
```

## Building from source
The language consists of two main components an interpreter in C and a compiler using the LLVM compiler toolchain in C++. Before you try to build either of these you will need to initialize the git submodules:
```sh
git submodule update --init
```

### Building the compiler
To build the compiler you will first need to build llvm from source, note that this which will take a while. After initializing llvm as a submodule change your working directory into that submodule:
```sh
cd lib/llvm-project
```
Then follow [LLVM's instructions for building from source](https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm)

Then simply run make from within the `/compiler/` directory:
```sh
cd compiler
make
```
Then you should end up with the `kebab` executable. This can be used to compile kebab (`.keb`) files into IR (`.ll` files).

If you want to run the tests you will also need to build googletest from source. After initializing googletest as a submodule change your working directory into that submodule:
```sh
cd lib/googletest
```
Then follow [GoogleTest's instructions for building from source](https://github.com/google/googletest/tree/main/googletest#readme)

Then simply run make from within the `/compiler/test/` directory, and run the `run_tests` executable.
```sh
cd compiler/test
make
./run_tests
```

### Building the interpreter
To build the intepreter you will first need to build nonstdlib from source. After initializing nonstdlib as a submodule change your working directory into the `src` directory under that submodule and run make:
```sh
cd lib/nonstdlib/src
make
```

Then simply run `make` from the `interpreter` directory:
```sh
cd interpreter
make
```
Then you should end up with the `kebab` executable. This can be used to interpret kebab (`.keb`) files with its own runtime.

## Typing
Kebab is a statically and strongly typed language. You define types for everything, including the types for parameters, return values, lists, etc. These types are enforced at runtime/compiletime and there is no `any` type. You can (and must) apply these types for every parameter and variable, through either specifying a variables constructor, or its type (more on constructors vs. type declarations later). Just to showcase some the language's advanced typing features here are some slightly absurd function definitions. 

```clj
; Functions can return other functions
def ret-fn = fn((outer : int) => fn((inner : int) =>
    int(outer + inner)
))

; Functions can take functions as parameters
def takes-fn = fn((fn-param : fn((int) => string)) => string(
  fn-param(42)
))

; Functions can take lists of functions as parameters
def takes-fn = fn((fn-params : list(fn(int) => string)) => string(
  fn-params[0](42)
))

; A function that takes an int as a parameter and returns a function takes
; an int as a parameter that returns a list of ints
def ret-fn-rets-list = fn((outer : int) => fn((inner : int) => list((int) => 
  [outer, inner]
)))

; In this signature we can see that `my-function` takes an int and a string as a parameter
; and returns a string. But from the constructor of `takes-fn` we can see that the function
; is supposed to return an int. Therefore returning the result of calling my-function is
; a type error since its return value of `string` is not the right type (`int`) for the function
def takes-fn = fn((my-function : fn(int, string) => string) => int(
  my-function(4, "hello") ; Error here - my-function does not return an int
))
```

## Constructors
### Primitives
Primitive constructors follow this pattern:
```
type(<constructor-body>)
```
Where the constructor body is any sequence of statements where the last one must be an expression (the return value of the constructor). Some primitive constructors are `char`, `bool`, `int` and `string`. Here are some examples
```clj
int(
  def a = 2
  a + 2
)
bool(true)
string("hello-world")
```

### Lists
List constructors follow this pattern:
```
list((<type>) => <constructor-body>)
```
Here are some examples
```clj
list((string) => ["hello", ",", "world"])

list((list(string)) => [
  ["hello", "world"],
  ["this", "is", "kebab"], ; Trailing commas are allowed ;)
])

list((fn(int) => string) => [
  fn((a : int) => int(a)),
  fn((s : string) => string(s)),
])
```

### Functions
Function constructors follow this pattern:
```
fn((<parameters>) => <constructor>)
```
Where the list of parameters is a comma separated list where each element should look like `<name> : <type>`. NOTE: The space before the `:` is very important here, as if you were to omit it, it would be included as a part of the name of the parameter which would cause a syntax error. The constructor in the functions body could be any other constructor, including another function constructor.

## Constructors vs. type declarations
There are two ways through which kebab gets information about types. The first is through constructor calls. These are some examples of constructor type inference.
```clj
; int constructor means a is now bound to the int type
def a = int(2)
; string constructor means b is now bound to the string type
def s = string("hello")

; list constructor, parametrized to be of type `string`. Here the list(...)
; is the constructor call, while the string inside the list constructor
; is a type declaration the list constructor uses to bind it to that type
def b = list((string) => ["hello", "world"])
```

The strongest combination of constructors and type declarations are seen in functions. Let's analyze this function:
```clj
def my-function = fn((a : int, l : list(string)) => int(
  a + 5
))
```
1. First we enter the `fn` constructor.
2. Then we parse the parameters of the function.
    1. The first parameter is called `a` and is declared to be of type `int`
    2. The second parameter is called `l` and is declared to be of type `list`.
        1. The list is parametrized to be of type string.
3. Then we parse the function body by calling another constructor - in this case the `int` constructor. This constructor also gives us the return type of the funciton, meaning this function must return an `int`.

## Mutability
Variables are constant by default, but can be made mutable if you add the `mut` qualifier when defining it. To mutate a mutable variable use a `set` statement.
```clj
; This is fine since my-var is mutable
def mut my-var = int(4)
set my-var = int(6) ; my-var is now 6

; This will error since my-const is constant
def my-const = int(4) ; This is fine
set my-const = int(6) ; Error here
```

## Conditionals
You can control the flow of your program with if/elif/else expressions - yes these are expressions. An if/elif/else expression must return a value and therefore always needs to have an else branch. This is how you can use them in kebab.
```clj
; a = 1
def a = int(
  if false => 0
  elif 2 == 2 => -1
  else => 1
)

```

You can also define variables local to a branch or do other calculations inside each branch. For example:
```clj
; c = [3, 6]
def c = list((int) =>
  def q = int(1 + 2)
  if 2 == 2 =>
    ; `w` is local to this if branch
    def w = int(6)
    [q, w]
  elif 1 == 2 =>
    printf("hello, world\n")
    [q, 1]
  else =>
    [1, 2]
)
```
