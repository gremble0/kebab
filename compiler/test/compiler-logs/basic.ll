; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@1 = private unnamed_addr constant [7 x i8] c"hello\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @main() {
entry:
  %inner = alloca ptr, align 8
  store ptr @0, ptr %inner, align 8
  %0 = load ptr, ptr %inner, align 8
  %my-num = alloca i64, align 8
  store i64 2, ptr %my-num, align 8
  %1 = load i64, ptr %my-num, align 4
  %2 = call i64 (ptr, ...) @printf(ptr @1)
  %v = alloca i64, align 8
  store i64 123, ptr %v, align 8
  %3 = load i64, ptr %v, align 4
  ret i64 0
}
