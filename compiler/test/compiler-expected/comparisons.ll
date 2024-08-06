; ModuleID = 'kebab'
source_filename = "kebab"

declare i64 @printf(ptr, ...)

define i64 @main({} %closure-env) {
entry:
  %b1 = alloca i1, align 1
  store i1 true, ptr %b1, align 1
  %b2 = alloca i1, align 1
  store i1 true, ptr %b2, align 1
  %b3 = alloca i1, align 1
  store i1 false, ptr %b3, align 1
  %b4 = alloca i1, align 1
  store i1 true, ptr %b4, align 1
  %b5 = alloca i1, align 1
  store i1 false, ptr %b5, align 1
  %b6 = alloca i1, align 1
  store i1 false, ptr %b6, align 1
  %b7 = alloca i1, align 1
  store i1 false, ptr %b7, align 1
  %b8 = alloca i1, align 1
  store i1 false, ptr %b8, align 1
  %b9 = alloca i1, align 1
  store i1 false, ptr %b9, align 1
  %b10 = alloca i1, align 1
  store i1 true, ptr %b10, align 1
  %b11 = alloca i1, align 1
  store i1 true, ptr %b11, align 1
  %b12 = alloca i1, align 1
  store i1 true, ptr %b12, align 1
  %i1 = alloca i64, align 8
  store i64 2, ptr %i1, align 8
  %i2 = alloca i64, align 8
  store i64 5, ptr %i2, align 8
  %0 = load i64, ptr %i1, align 4
  %1 = load i64, ptr %i2, align 4
  %2 = icmp ult i64 %0, %1
  %b13 = alloca i1, align 1
  store i1 %2, ptr %b13, align 1
  %f1 = alloca double, align 8
  store double 2.500000e+00, ptr %f1, align 8
  %f2 = alloca double, align 8
  store double 2.200000e+00, ptr %f2, align 8
  %3 = load double, ptr %f1, align 8
  %4 = load double, ptr %f2, align 8
  %5 = fcmp ult double %3, %4
  %b14 = alloca i1, align 1
  store i1 %5, ptr %b14, align 1
  ret i64 0
}
