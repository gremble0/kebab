; ModuleID = 'kebab'
source_filename = "kebab"

declare i64 @printf(ptr, ...)

define i64 @main({} %0) {
entry:
  %i1 = alloca i64, align 8
  store i64 1, ptr %i1, align 8
  %1 = load i64, ptr %i1, align 8
  %i2 = alloca i64, align 8
  store i64 6, ptr %i2, align 8
  %2 = load i64, ptr %i2, align 8
  %i3 = alloca i64, align 8
  store i64 -10, ptr %i3, align 8
  %3 = load i64, ptr %i3, align 8
  %f1 = alloca double, align 8
  store double 7.750000e+00, ptr %f1, align 8
  %4 = load double, ptr %f1, align 8
  %f2 = alloca double, align 8
  store double 0x4003A2E8BA2E8BA3, ptr %f2, align 8
  %5 = load double, ptr %f2, align 8
  ret i64 0
}
