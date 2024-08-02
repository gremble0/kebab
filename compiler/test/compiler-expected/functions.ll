; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1

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

define i64 @takes-parameter(i64 %n, {} %__closure_env) {
entry:
  ret i64 0
}

define i64 @uses-parameter(i64 %n, {} %__closure_env) {
entry:
  ret i64 %n
}

define i64 @has-local-fn({} %__closure_env) {
entry:
  %0 = call i64 @local-fn({} undef)
  ret i64 %0
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
  %5 = call i64 (ptr, ...) @printf(ptr @0, i64 %4)
  %i = alloca i64, align 8
  store i64 %5, ptr %i, align 8
  %6 = load i64, ptr %i, align 8
  ret i64 0
}
