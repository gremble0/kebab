; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [14 x i8] c"hello, world\0A\00", align 1
@s1 = global ptr @0
@1 = private unnamed_addr constant [16 x i8] c"\0Ahello\0A, world\0A\00", align 1
@s2 = global ptr @1
@c1 = global i8 39
@c2 = global i8 10

declare i32 @printf(ptr, ...)

define i32 @main(i32 %0, ptr %1) {
entry:
  ret i32 0
}
