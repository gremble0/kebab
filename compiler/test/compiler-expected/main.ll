; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [11 x i8] c"hello %ld\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @main(i64 %argc, {} %0) {
entry:
  %1 = call i64 (ptr, ...) @printf(ptr @0, i64 %argc)
  ret i64 0
}
