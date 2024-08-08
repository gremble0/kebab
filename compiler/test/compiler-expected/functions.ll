; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@1 = private unnamed_addr constant [39 x i8] c"result of calling local function: %ld\0A\00", align 1
@2 = private unnamed_addr constant [17 x i8] c"two + two = %ld\0A\00", align 1
@3 = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@4 = private unnamed_addr constant [39 x i8] c"i1: %ld\0Ai2: %ld\0Ai3: %ld\0Ai4: %ld\0As: %s\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @one-factory({} %closure-env) {
entry:
  ret i64 1
}

define i64 @local-addition({} %closure-env) {
entry:
  %n = alloca i64, align 8
  store i64 68, ptr %n, align 8
  %0 = load i64, ptr %n, align 8
  %1 = add i64 %0, 1
  ret i64 %1
}

define i64 @function-consumer({} %closure-env) {
entry:
  %0 = call i64 @one-factory({} undef)
  %one = alloca i64, align 8
  store i64 %0, ptr %one, align 8
  %1 = load i64, ptr %one, align 8
  ret i64 %1
}

define ptr @takes-parameter(ptr %s, {} %closure-env) {
entry:
  %"arg:s" = alloca ptr, align 8
  store ptr %s, ptr %"arg:s", align 8
  ret ptr @0
}

define i64 @uses-parameter(i64 %n, {} %closure-env) {
entry:
  %"arg:n" = alloca i64, align 8
  store i64 %n, ptr %"arg:n", align 8
  %0 = load i64, ptr %"arg:n", align 8
  ret i64 %0
}

define i64 @has-local-fn({} %closure-env) {
entry:
  %0 = call i64 @local-fn({} undef)
  %1 = call i64 (ptr, ...) @printf(ptr @1, i64 %0)
  ret i64 0
}

define i64 @local-fn({} %closure-env) {
entry:
  ret i64 2
}

define i64 @main({} %closure-env) {
entry:
  %0 = call i64 @one-factory({} undef)
  %1 = call i64 @one-factory({} undef)
  %2 = add i64 %0, %1
  %two = alloca i64, align 8
  store i64 %2, ptr %two, align 8
  %3 = load i64, ptr %two, align 8
  %4 = load i64, ptr %two, align 8
  %5 = add i64 %3, %4
  %6 = call i64 (ptr, ...) @printf(ptr @2, i64 %5)
  %7 = call i64 @local-addition({} undef)
  %i1 = alloca i64, align 8
  store i64 %7, ptr %i1, align 8
  %8 = call i64 @function-consumer({} undef)
  %i2 = alloca i64, align 8
  store i64 %8, ptr %i2, align 8
  %9 = call i64 @uses-parameter(i64 42, {} undef)
  %i3 = alloca i64, align 8
  store i64 %9, ptr %i3, align 8
  %10 = call i64 @has-local-fn({} undef)
  %i4 = alloca i64, align 8
  store i64 %10, ptr %i4, align 8
  %11 = call ptr @takes-parameter(ptr @3, {} undef)
  %s = alloca ptr, align 8
  store ptr %11, ptr %s, align 8
  %12 = load i64, ptr %i1, align 8
  %13 = load i64, ptr %i2, align 8
  %14 = load i64, ptr %i3, align 8
  %15 = load i64, ptr %i4, align 8
  %16 = load ptr, ptr %s, align 8
  %17 = call i64 (ptr, ...) @printf(ptr @4, i64 %12, i64 %13, i64 %14, i64 %15, ptr %16)
  ret i64 0
}
