; ModuleID = 'kebab'
source_filename = "kebab"

declare i32 @printf(ptr, ...)

define i32 @main(i32 %0, ptr %1) {
entry:
  %inner = alloca i64, align 8
  store i64 0, ptr %inner, align 4
  %a = alloca i64, align 8
  store ptr %inner, ptr %a, align 8
  ret i32 0
}
