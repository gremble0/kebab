; ModuleID = 'kebab'
source_filename = "kebab"

declare i64 @printf(ptr, ...)

define i64 @main({} %closure-env) {
entry:
  %b1 = alloca i1, align 1
  store i1 true, ptr %b1, align 1
  %b2 = alloca i1, align 1
  store i1 true, ptr %b2, align 1
  %b3 = alloca i1, align 1
  store i1 false, ptr %b3, align 1
  %b4 = alloca i1, align 1
  store i1 false, ptr %b4, align 1
  %b5 = alloca i1, align 1
  store i1 false, ptr %b5, align 1
  %b6 = alloca i1, align 1
  store i1 false, ptr %b6, align 1
  ret i64 0
}
