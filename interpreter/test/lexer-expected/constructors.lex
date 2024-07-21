def
name: "add"
=
name: "int"
(
integer-literal: 2
+
integer-literal: 2
)
def
name: "kebab-case"
=
fn
(
(
name: "var"
:
name: "composite-type"
)
=>
name: "int"
(
name: "var"
)
)
def
mut
name: "my-var"
=
name: "int"
(
integer-literal: 4
)
set
name: "my-var"
=
name: "int"
(
integer-literal: 6
)
def
name: "my-const"
=
name: "int"
(
integer-literal: 4
)
set
name: "my-const"
=
name: "int"
(
integer-literal: 6
)
def
name: "ch"
=
name: "char"
(
char-literal: 'c'
)
def
name: "str"
=
name: "string"
(
string-literal: "hey"
)
def
name: "lst"
=
list
(
name: "int"
=>
integer-literal: 1
,
integer-literal: 2
,
integer-literal: 3
)
def
name: "c"
=
fn
(
(
name: "num1"
:
name: "int"
,
name: "nums"
:
list
(
name: "int"
)
)
=>
name: "int"
(
name: "num1"
+
integer-literal: 2
)
)
