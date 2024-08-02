; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [6 x i8] c"world\00", align 1
@1 = private unnamed_addr constant [26 x i8] c"hello %s, local-var: %ld\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @main({} %__closure_env) {
entry:
  %local-var = alloca i64, align 8
  store i64 69, ptr %local-var, align 8
  %0 = load i64, ptr %local-var, align 8
  %local-string = alloca ptr, align 8
  store ptr @0, ptr %local-string, align 8
  %1 = load ptr, ptr %local-string, align 8
  %2 = insertvalue { ptr, i64 } undef, ptr %1, 0
  %__closure_arg = insertvalue { ptr, i64 } %2, i64 %0, 1
  %3 = call i64 @local-fn({ ptr, i64 } %__closure_arg)
  %local-fn-called = alloca i64, align 8
  store i64 %3, ptr %local-fn-called, align 8
  %4 = load i64, ptr %local-fn-called, align 8
  ret i64 0
}

define i64 @local-fn({ ptr, i64 } %__closure_env) {
entry:
  %0 = extractvalue { ptr, i64 } %__closure_env, 0
  %1 = extractvalue { ptr, i64 } %__closure_env, 1
  %inner-var = alloca i64, align 8
  store i64 42, ptr %inner-var, align 8
  %2 = load i64, ptr %inner-var, align 8
  %3 = add i64 %2, %1
  %4 = call i64 (ptr, ...) @printf(ptr @1, ptr %0, i64 %3)
  %5 = add i64 %2, %1
  ret i64 %5
}
