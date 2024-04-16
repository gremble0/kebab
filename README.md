# Kebab
Kebab is a primarily functional programming language that I'm currently developing (just finished the parser so there is not yet any runtime). Here is how it looks:

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
def l1 = list(int => 1, 2, 3)
def l2 = list(string => "123", "gg", "hhh")
def add-one = fn((a : int) => int(a + 1)) ; function that takes an int and returns an int
def b1 = bool(1 == -2) ; false
def b2 = bool(1 ~= -2) ; true

; Nested statements inside constructors. The constructor returns on the first
; expression (in this case `2 + nested` which evaluates to 7).
def nested-example = int(def nested = int(5)
                         2 + nested)

```

## Typing
Kebab is a strongly typed language. You can define types for everything, including the types for parameters, return values, lists, etc. You can (and must) apply these types for every parameter and variable, through either specifying a variables constructor, or its type (more on constructors vs. type declarations later). Just to showcase some the language's advanced typing features here are some slightly absurd function definitions. 

```clj
; A function that returns a function that takes an int as a parameter and
; returns an int
def ret-fn = fn((outer : int) => fn((inner : int) => int(outer + inner)))

; A function that takes a function that takes a list of ints and returns
;a string as a parameter and returns a char
def takes-fn = fn((param : list(fn(list(int)) => string)) => char('c'))

; A function that takes an int as a parameter and returns a function takes
; an int as a parameter that returns a list of ints
def ret-fn-rets-list = fn((outer : int) => fn((inner : int) => list(int => 
  outer, inner
)))

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

; You get the point :)
```

## Constructors vs. type declarations
There are two ways through which kebab, gets information about types. The first is through constructor calls. These are some examples of constructor type inference.
```clj
; int constructor means a is now bound to the int type
def a = int(2)
; string constructor means b is now bound to the string type
def s = string("hello")

; list constructor, parametrized to be of type `string`. Here the list(...)
; is the constructor call, while the string inside the list constructor
; is a type declaration the list constructor uses to bind it to that type
def b = list(string => "hello", "world")
```

The strongest combination of constructors and type declarations are seen in functions. Let's analyze this function:
```clj
def my-function = fn((a : int, l : list(string)) => int(
  a + 5
))
```
1. First we call the `fn` constructor.
2. Then we parse the parameters of the function.
    1. The first parameter is called `a` and is declared to be of type `int`
    2. The second parameter is called `l` and is declared to be of type `list`.
        1. The list is parametrized to be of type string.
3. Then we parse the function body by calling another constructor - in this case the `int` constructor. This constructor also gives us the return type of the funciton, meaning this function must return an `int`.

## How to use constructors
### Primitives
Primitive constructors allows for any number of statements, including local `def` bindings and other functions with side effects, but will return the first expression in its body. Some primitive constructors are `char`, `bool`, `int` and `string`. You can use them like this:
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
list(string => ...)

; A list of other lists of strings
list(list(string) => ...)

; A list of functions that takes an int as a parameter and returns a string.
list(fn(int) => string => ...)
```
Following the parametrized type of the list should be a `=>` following a comma separated list of the contents of the list. Lists do not support nested statements (maybe they could).


## Mutability
Variables are constant by default, but mutable if you add the `mut` qualifier. To change a mutable variable use a `set` statement.
```clj
; This is fine
def mut my-var = int(4)
set my-var = int(6) ; my-var is now 6

; This should error
def my-const = int(4)
set my-const = int(6) ; Error here
```
