; ModuleID = 'kebab'
source_filename = "kebab"

declare i64 @printf(ptr, ...)

define i64 @main() {
entry:
  %b1 = alloca i1, align 1
  store i1 false, ptr %b1, align 1
  %0 = load i1, ptr %b1, align 1
  %b2 = alloca i1, align 1
  store i1 false, ptr %b2, align 1
  %1 = load i1, ptr %b2, align 1
  %b3 = alloca i1, align 1
  store i1 true, ptr %b3, align 1
  %2 = load i1, ptr %b3, align 1
  ret i64 0
}
