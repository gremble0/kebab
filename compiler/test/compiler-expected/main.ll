; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [11 x i8] c"hello %ld\0A\00", align 1

declare i64 @printf(ptr, ...)

declare ptr @malloc(i64)

define i64 @main(i64 %argc, {} %closure-env) {
entry:
  %"arg:argc" = alloca i64, align 8
  store i64 %argc, ptr %"arg:argc", align 8
  %0 = load i64, ptr %"arg:argc", align 8
  %1 = call i64 (ptr, ...) @printf(ptr @0, i64 %0)
  ret i64 0
}
