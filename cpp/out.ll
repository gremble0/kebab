; ModuleID = 'kebab'
source_filename = "kebab"

@b = global i64 2, align 4
@c = global [7 x i8] c"hello\22\00", align 4
@d = global ptr @c, align 4
@a = global i64 4, align 4

declare i32 @printf(ptr, ...)

define i32 @main(i32 %0, ptr %1) {
entry:
  ret i32 0
}
