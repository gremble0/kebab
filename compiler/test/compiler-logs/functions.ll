; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @one-factory() {
entry:
  ret i64 1
}

define i64 @local-addition() {
entry:
  %n = alloca i64, align 8
  store i64 68, ptr %n, align 8
  %0 = load i64, ptr %n, align 4
  %1 = add i64 %0, 1
  ret i64 %1
}

define i64 @function-consumer() {
entry:
  %0 = call i64 @one-factory()
  %one = alloca i64, align 8
  store i64 %0, ptr %one, align 8
  %1 = load i64, ptr %one, align 4
  ret i64 %1
}

define i64 @takes-parameter(i64 %n) {
entry:
  ret i64 0
}

define i64 @uses-parameter(i64 %n) {
entry:
  ret i64 %n
}

define i64 @has-local-fn() {
entry:
  %0 = call i64 @local-fn()
  ret i64 %0
}

define i64 @local-fn() {
entry:
  ret i64 2
}

define i64 @main() {
entry:
  %0 = call i64 @one-factory()
  %1 = call i64 @one-factory()
  %2 = add i64 %0, %1
  %two = alloca i64, align 8
  store i64 %2, ptr %two, align 8
  %3 = load i64, ptr %two, align 4
  %4 = add i64 %3, %3
  %5 = call i64 (ptr, ...) @printf(ptr @0, i64 %4)
  %i = alloca i64, align 8
  store i64 %5, ptr %i, align 8
  %6 = load i64, ptr %i, align 4
  ret i64 0
}
