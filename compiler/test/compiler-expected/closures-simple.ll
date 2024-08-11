; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [6 x i8] c"world\00", align 1
@1 = private unnamed_addr constant [26 x i8] c"hello %s, local-var: %ld\0A\00", align 1

declare i64 @printf(ptr, ...)

declare ptr @malloc(i64)

define i64 @main({} %closure-env) {
entry:
  %local-var = alloca i64, align 8
  store i64 69, ptr %local-var, align 8
  %local-string = alloca ptr, align 8
  store ptr @0, ptr %local-string, align 8
  %0 = insertvalue { ptr, ptr } undef, ptr %local-var, 0
  %closure-arg = insertvalue { ptr, ptr } %0, ptr %local-string, 1
  %1 = call i64 @local-fn({ ptr, ptr } %closure-arg)
  %local-fn-called = alloca i64, align 8
  store i64 %1, ptr %local-fn-called, align 8
  ret i64 0
}

define i64 @local-fn({ ptr, ptr } %closure-env) {
entry:
  %"closure-env:local-var" = extractvalue { ptr, ptr } %closure-env, 0
  %"closure-env:local-var-ptr" = alloca ptr, align 8
  store ptr %"closure-env:local-var", ptr %"closure-env:local-var-ptr", align 8
  %0 = load ptr, ptr %"closure-env:local-var-ptr", align 8
  %"closure-env:local-string" = extractvalue { ptr, ptr } %closure-env, 1
  %"closure-env:local-string-ptr" = alloca ptr, align 8
  store ptr %"closure-env:local-string", ptr %"closure-env:local-string-ptr", align 8
  %1 = load ptr, ptr %"closure-env:local-string-ptr", align 8
  %inner-var = alloca i64, align 8
  store i64 42, ptr %inner-var, align 8
  %2 = load ptr, ptr %1, align 8
  %3 = load i64, ptr %inner-var, align 8
  %4 = load i64, ptr %0, align 8
  %5 = add i64 %3, %4
  %6 = call i64 (ptr, ...) @printf(ptr @1, ptr %2, i64 %5)
  %7 = load i64, ptr %inner-var, align 8
  %8 = load i64, ptr %0, align 8
  %9 = add i64 %7, %8
  ret i64 %9
}
