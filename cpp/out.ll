; ModuleID = 'kebab'
source_filename = "kebab"

declare i32 @printf(ptr, ...)

define i32 @main(i32 %0, ptr %1) {
entry:
  %inner = alloca i64, align 8
  store i64 0, ptr %inner, align 4
  %f = alloca float, align 4
  store float 0x3FECCCCCC0000000, ptr %f, align 4
  %a = alloca i64, align 8
  store i64 42, ptr %a, align 4
  %2 = alloca [7 x i8], align 1
  store [7 x i8] c"hello\22\00", ptr %2, align 1
  %s = alloca ptr, align 8
  store ptr %2, ptr %s, align 8
  ret i32 0
}
