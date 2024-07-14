; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@inner = global ptr @0
@my-num = global i64 2
@1 = private unnamed_addr constant [7 x i8] c"hello\0A\00", align 1
@v = global i32 %2

declare i32 @printf(ptr, ...)

define i32 @main(i32 %0, ptr %1) {
entry:
  %2 = call i32 (ptr, ...) @printf(ptr @1)
  ret i32 0
}
