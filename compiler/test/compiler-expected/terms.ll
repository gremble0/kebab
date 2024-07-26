; ModuleID = 'kebab'
source_filename = "kebab"

declare i64 @printf(ptr, ...)

define i64 @main() {
entry:
  %i1 = alloca i64, align 8
  store i64 6, ptr %i1, align 8
  %0 = load i64, ptr %i1, align 8
  %i2 = alloca i64, align 8
  store i64 3, ptr %i2, align 8
  %1 = load i64, ptr %i2, align 8
  %2 = sub i64 %0, %1
  %3 = add i64 %2, 3
  %i3 = alloca i64, align 8
  store i64 %3, ptr %i3, align 8
  %4 = load i64, ptr %i3, align 8
  %f1 = alloca double, align 8
  store double 2.300000e+00, ptr %f1, align 8
  %5 = load double, ptr %f1, align 8
  %f2 = alloca double, align 8
  store double 2.400000e+00, ptr %f2, align 8
  %6 = load double, ptr %f2, align 8
  ret i64 0
}
