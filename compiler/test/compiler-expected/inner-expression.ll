; ModuleID = 'kebab'
source_filename = "kebab"

declare i64 @printf(ptr, ...)

define i64 @main({} %closure-env) {
entry:
  %local = alloca i64, align 8
  store i64 10, ptr %local, align 8
  %0 = load i64, ptr %local, align 8
  %local2 = alloca i64, align 8
  store i64 -2, ptr %local2, align 8
  %1 = load i64, ptr %local2, align 8
  ret i64 0
}
