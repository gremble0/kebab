; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@1 = private unnamed_addr constant [39 x i8] c"result of calling local function: %ld\0A\00", align 1
@2 = private unnamed_addr constant [17 x i8] c"two + two = %ld\0A\00", align 1
@3 = private unnamed_addr constant [39 x i8] c"i1: %ld\0Ai2: %ld\0Ai3: %ld\0Ai4: %ld\0As: %s\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @one-factory({} %__closure_env) {
entry:
  ret i64 1
}

define i64 @local-addition({} %__closure_env) {
entry:
  %n = alloca i64, align 8
  store i64 68, ptr %n, align 8
  %0 = load i64, ptr %n, align 8
  %1 = add i64 %0, 1
  ret i64 %1
}

define i64 @function-consumer({} %__closure_env) {
entry:
  %0 = call i64 @one-factory({} undef)
  %one = alloca i64, align 8
  store i64 %0, ptr %one, align 8
  %1 = load i64, ptr %one, align 8
  ret i64 %1
}

define ptr @takes-parameter(ptr %s, {} %__closure_env) {
entry:
  ret ptr @0
}

define i64 @uses-parameter(i64 %n, {} %__closure_env) {
entry:
  ret i64 %n
}

define i64 @has-local-fn({} %__closure_env) {
entry:
  %0 = call i64 @local-fn({} undef)
  %1 = call i64 (ptr, ...) @printf(ptr @1, i64 %0)
  ret i64 0
}

define i64 @local-fn({} %__closure_env) {
entry:
  ret i64 2
}

define i64 @main({} %__closure_env) {
entry:
  %0 = call i64 @one-factory({} undef)
  %1 = call i64 @one-factory({} undef)
  %2 = add i64 %0, %1
  %two = alloca i64, align 8
  store i64 %2, ptr %two, align 8
  %3 = load i64, ptr %two, align 8
  %4 = add i64 %3, %3
  %5 = call i64 (ptr, ...) @printf(ptr @2, i64 %4)
  %6 = call i64 @local-addition({} undef)
  %i1 = alloca i64, align 8
  store i64 %6, ptr %i1, align 8
  %7 = load i64, ptr %i1, align 8
  %8 = call i64 @function-consumer({} undef)
  %i2 = alloca i64, align 8
  store i64 %8, ptr %i2, align 8
  %9 = load i64, ptr %i2, align 8
  %10 = call i64 @uses-parameter(i64 42, {} undef)
  %i3 = alloca i64, align 8
  store i64 %10, ptr %i3, align 8
  %11 = load i64, ptr %i3, align 8
  %12 = call i64 @has-local-fn({} undef)
  %i4 = alloca i64, align 8
  store i64 %12, ptr %i4, align 8
  %13 = load i64, ptr %i4, align 8
  %14 = call ptr @takes-parameter({} undef)
  %s = alloca ptr, align 8
  store ptr %14, ptr %s, align 8
  %15 = load ptr, ptr %s, align 8
  %16 = call i64 (ptr, ...) @printf(ptr @3, i64 %7, i64 %9, i64 %11, i64 %13, ptr %15)
  ret i64 0
}
