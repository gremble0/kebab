; ModuleID = 'kebab'
source_filename = "kebab"

declare i64 @printf(ptr, ...)

declare ptr @malloc(i64)

define i64 @main({} %closure-env) {
entry:
  %local = alloca i64, align 8
  store i64 10, ptr %local, align 8
  %local2 = alloca i64, align 8
  store i64 -2, ptr %local2, align 8
  ret i64 0
}
