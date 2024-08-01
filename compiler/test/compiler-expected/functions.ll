; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @one-factory({} %0) {
entry:
  ret i64 1
}

define i64 @local-addition({} %0) {
entry:
  %n = alloca i64, align 8
  store i64 68, ptr %n, align 8
  %1 = load i64, ptr %n, align 8
  %2 = add i64 %1, 1
  ret i64 %2
}

define i64 @function-consumer({} %0) {
entry:
  %1 = call i64 @one-factory({} undef)
  %one = alloca i64, align 8
  store i64 %1, ptr %one, align 8
  %2 = load i64, ptr %one, align 8
  ret i64 %2
}

define i64 @takes-parameter(i64 %n, {} %0) {
entry:
  ret i64 0
}

define i64 @uses-parameter(i64 %n, {} %0) {
entry:
  ret i64 %n
}

define i64 @has-local-fn({} %0) {
entry:
  %1 = call i64 @local-fn({} undef)
  ret i64 %1
}

define i64 @local-fn({} %0) {
entry:
  ret i64 2
}

define i64 @main({} %0) {
entry:
  %1 = call i64 @one-factory({} undef)
  %2 = call i64 @one-factory({} undef)
  %3 = add i64 %1, %2
  %two = alloca i64, align 8
  store i64 %3, ptr %two, align 8
  %4 = load i64, ptr %two, align 8
  %5 = add i64 %4, %4
  %6 = call i64 (ptr, ...) @printf(ptr @0, i64 %5)
  %i = alloca i64, align 8
  store i64 %6, ptr %i, align 8
  %7 = load i64, ptr %i, align 8
  ret i64 0
}
