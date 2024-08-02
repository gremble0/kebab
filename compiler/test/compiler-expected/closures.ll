; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [6 x i8] c"world\00", align 1
@1 = private unnamed_addr constant [26 x i8] c"hello %s, local-var: %ld\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @main({} %0) {
entry:
  %local-var = alloca i64, align 8
  store i64 69, ptr %local-var, align 8
  %1 = load i64, ptr %local-var, align 8
  %local-string = alloca ptr, align 8
  store ptr @0, ptr %local-string, align 8
  %2 = load ptr, ptr %local-string, align 8
  %3 = insertvalue { ptr, i64 } undef, ptr %2, 0
  %4 = insertvalue { ptr, i64 } %3, i64 %1, 1
  %5 = call i64 @local-fn({ ptr, i64 } %4)
  %local-fn-called = alloca i64, align 8
  store i64 %5, ptr %local-fn-called, align 8
  %6 = load i64, ptr %local-fn-called, align 8
  ret i64 0
}

define i64 @local-fn({ ptr, i64 } %0) {
entry:
  %1 = extractvalue { ptr, i64 } %0, 0
  %2 = extractvalue { ptr, i64 } %0, 1
  %inner-var = alloca i64, align 8
  store i64 42, ptr %inner-var, align 8
  %3 = load i64, ptr %inner-var, align 8
  %4 = add i64 %3, %2
  %5 = call i64 (ptr, ...) @printf(ptr @1, ptr %1, i64 %4)
  %6 = add i64 %3, %2
  ret i64 %6
}
