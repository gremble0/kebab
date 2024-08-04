; ModuleID = 'kebab'
source_filename = "kebab"

declare i64 @printf(ptr, ...)

define i64 @main({} %closure-env) {
entry:
  %0 = alloca [3 x i64], i64 3, align 8
  %1 = getelementptr inbounds [3 x i64], ptr %0, i64 0, i64 0
  store i64 1, ptr %1, align 4
  %2 = getelementptr inbounds [3 x i64], ptr %0, i64 0, i64 1
  store i64 2, ptr %2, align 4
  %3 = getelementptr inbounds [3 x i64], ptr %0, i64 0, i64 2
  store i64 3, ptr %3, align 4
  %4 = load [3 x i64], ptr %0, align 8
  %numbers = alloca [3 x i64], align 8
  store [3 x i64] %4, ptr %numbers, align 8
  %5 = load [3 x i64], ptr %numbers, align 8
  %6 = getelementptr inbounds [3 x i64], ptr %numbers, i64 0, i64 1
  %7 = load i64, ptr %6, align 4
  %number = alloca i64, align 8
  store i64 %7, ptr %number, align 8
  %8 = load i64, ptr %number, align 8
  ret i64 0
}
