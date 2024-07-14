# Kebab
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
def l1 = list((int) => [1, 2, 3]) ; list of three ints
def l2 = list((string) => ["123", "gg", "hhh"]) ; list of three strings
def add-one = fn((a : int) => int(a + 1)) ; function that takes an int and returns an int
def b1 = bool(1 == -2) ; false
def b2 = bool(1 ~= -2) ; true

; Nested statements inside constructors. The constructor returns on the first
; expression (in this case `2 + nested` which evaluates to 7).
def nested-example = int(def nested = int(5)
                         2 + nested)

```

## Typing
Kebab is a strongly typed language. You can define types for everything, including the types for parameters, return values, lists, etc. These types are enforced at runtime and there is no `any` type. You can (and must) apply these types for every parameter and variable, through either specifying a variables constructor, or its type (more on constructors vs. type declarations later). Just to showcase some the language's advanced typing features here are some slightly absurd function definitions. 

```clj
; A function that returns a function that takes an int as a parameter and
; returns an int
def ret-fn = fn((outer : int) => fn((inner : int) => int(outer + inner)))

; A function that takes a function that takes a list of ints and returns
;a string as a parameter and returns a char
def takes-fn = fn((param : list(fn(list(int)) => string)) => char('c'))

; A function that takes an int as a parameter and returns a function takes
; an int as a parameter that returns a list of ints
def ret-fn-rets-list = fn((outer : int) =>
  fn((inner : int) => list((int) => 
    outer, inner
  ))
)

; A function that takes a function that takes an int and a string and returns
; a string that returns an int from the result of calling its parameter function
;
; BUT: hold on:
; we see from the type declaration of the my-function parameter that this function
; returns a string, and through the int constructor of the takes-fn function we
; see that we should return an int. Therefore we can infer that this is a type error.
def takes-fn = fn((my-function : fn(int, string) => string) => int(
  my-function(4, "hello") ; Error here - my-function does not return an int
))
```

## Constructors
### Primitives
Primitive constructors allows for any number of statements, including local `def` bindings and other functions with side effects, but will return the last expression in its body (implicit return). Some primitive constructors are `char`, `bool`, `int` and `string`. You can use them like this:
```clj
int(def a = 2
    a + 2)
bool(true)
string("hello-world")
```

### Lists
List constructors must be parametrized to hold a certain type. This could be any type, including functions or more lists! For example:
```clj
; Simple list of string
list((string) => ...)

; A list of other lists of strings
list((list(string)) => ...)

; A list of functions that takes an int as a parameter and returns a string.
list((fn(int) => string) => ...)
```
Following the parametrized type of the list should be a `=>` following a comma separated list of the contents of the list. Lists do not support nested statements (maybe they could).

### Functions
Function constructors follow this pattern:
```
fn((<parameters>) => <constructor>)
```
Where the list of parameters is a comma separated list where each element should look like `<name> : <type>`. NOTE: The space before the `:` is very important here, as if you were to omit it, it would be included as a part of the name of the parameter which would cause a syntax error. The constructor in the functions body could be any other constructor.

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
