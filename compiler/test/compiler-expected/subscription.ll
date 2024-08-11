; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@1 = private unnamed_addr constant [6 x i8] c"world\00", align 1
@2 = private unnamed_addr constant [20 x i8] c"num: %ld, word: %s\0A\00", align 1

declare i64 @printf(ptr, ...)

declare ptr @malloc(i64)

define i64 @main({} %closure-env) {
entry:
  %0 = call ptr @malloc(i64 24)
  %1 = getelementptr i64, ptr %0, i64 0
  store i64 1, ptr %1, align 8
  %2 = getelementptr i64, ptr %0, i64 1
  store i64 2, ptr %2, align 8
  %3 = getelementptr i64, ptr %0, i64 2
  store i64 3, ptr %3, align 8
  %numbers = alloca ptr, align 8
  store ptr %0, ptr %numbers, align 8
  %4 = call ptr @malloc(i64 0)
  %5 = getelementptr ptr, ptr %4, i64 0
  store ptr @0, ptr %5, align 8
  %6 = getelementptr ptr, ptr %4, i64 1
  store ptr @1, ptr %6, align 8
  %words = alloca ptr, align 8
  store ptr %4, ptr %words, align 8
  %7 = load ptr, ptr %numbers, align 8
  %8 = getelementptr i64, ptr %7, i64 1
  %9 = load i64, ptr %8, align 8
  %number = alloca i64, align 8
  store i64 %9, ptr %number, align 8
  %10 = load ptr, ptr %words, align 8
  %11 = getelementptr ptr, ptr %10, i64 0
  %12 = load ptr, ptr %11, align 8
  %word = alloca ptr, align 8
  store ptr %12, ptr %word, align 8
  %13 = load i64, ptr %number, align 8
  %14 = load ptr, ptr %word, align 8
  %15 = call i64 (ptr, ...) @printf(ptr @2, i64 %13, ptr %14)
  ret i64 0
}
