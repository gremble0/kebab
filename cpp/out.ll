; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [8 x i8] c"hello\\n\00", align 1
@v = global ptr @printf

declare i32 @printf(ptr, ...)

define i32 @main(i32 %0, ptr %1) {
entry:
  %2 = call i32 (ptr, ...) @printf(ptr @0)
  ret i32 0
}
