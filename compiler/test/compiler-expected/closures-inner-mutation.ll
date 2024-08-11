; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1
@1 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1
@2 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1

declare i64 @printf(ptr, ...)

declare ptr @malloc(i64)

define i64 @main({} %closure-env) {
entry:
  %counter = alloca i64, align 8
  store i64 0, ptr %counter, align 8
  %0 = load i64, ptr %counter, align 8
  %1 = call i64 (ptr, ...) @printf(ptr @0, i64 %0)
  %closure-arg = insertvalue { ptr } undef, ptr %counter, 0
  %2 = call i64 @increment-counter({ ptr } %closure-arg)
  %3 = load i64, ptr %counter, align 8
  %4 = call i64 (ptr, ...) @printf(ptr @1, i64 %3)
  %closure-arg1 = insertvalue { ptr } undef, ptr %counter, 0
  %5 = call i64 @increment-counter({ ptr } %closure-arg1)
  %6 = load i64, ptr %counter, align 8
  %7 = call i64 (ptr, ...) @printf(ptr @2, i64 %6)
  ret i64 0
}

define i64 @increment-counter({ ptr } %closure-env) {
entry:
  %"closure-env:counter" = extractvalue { ptr } %closure-env, 0
  %"closure-env:counter-ptr" = alloca ptr, align 8
  store ptr %"closure-env:counter", ptr %"closure-env:counter-ptr", align 8
  %0 = load ptr, ptr %"closure-env:counter-ptr", align 8
  %1 = load i64, ptr %0, align 8
  %2 = add i64 %1, 1
  store i64 %2, ptr %0, align 8
  ret i64 0
}
