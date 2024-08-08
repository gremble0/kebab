; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1
@1 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1
@2 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @fib(i64 %n, {} %closure-env) {
entry:
  %"arg:n" = alloca i64, align 8
  store i64 %n, ptr %"arg:n", align 8
  br label %if_branch

if_branch:                                        ; preds = %entry
  %0 = load i64, ptr %"arg:n", align 8
  %1 = icmp ult i64 %0, 2
  %2 = icmp eq i1 %1, true
  %3 = load i64, ptr %"arg:n", align 8
  br i1 %2, label %merge_branch, label %else_branch

merge_branch:                                     ; preds = %else_branch, %if_branch
  %4 = phi i64 [ %3, %if_branch ], [ %11, %else_branch ]
  ret i64 %4

else_branch:                                      ; preds = %if_branch
  %5 = load i64, ptr %"arg:n", align 8
  %6 = sub i64 %5, 1
  %7 = call i64 @fib(i64 %6, {} undef)
  %8 = load i64, ptr %"arg:n", align 8
  %9 = sub i64 %8, 2
  %10 = call i64 @fib(i64 %9, {} undef)
  %11 = add i64 %7, %10
  br label %merge_branch
}

define i64 @fac(i64 %n, {} %closure-env) {
entry:
  %"arg:n" = alloca i64, align 8
  store i64 %n, ptr %"arg:n", align 8
  br label %if_branch

if_branch:                                        ; preds = %entry
  %0 = load i64, ptr %"arg:n", align 8
  %1 = icmp ule i64 %0, 1
  %2 = icmp eq i1 %1, true
  %3 = load i64, ptr %"arg:n", align 8
  br i1 %2, label %merge_branch, label %else_branch

merge_branch:                                     ; preds = %else_branch, %if_branch
  %4 = phi i64 [ %3, %if_branch ], [ %9, %else_branch ]
  ret i64 %4

else_branch:                                      ; preds = %if_branch
  %5 = load i64, ptr %"arg:n", align 8
  %6 = load i64, ptr %"arg:n", align 8
  %7 = sub i64 %6, 1
  %8 = call i64 @fac(i64 %7, {} undef)
  %9 = mul i64 %5, %8
  br label %merge_branch
}

define i64 @exp(i64 %base, i64 %exponent, {} %closure-env) {
entry:
  %"arg:base" = alloca i64, align 8
  store i64 %base, ptr %"arg:base", align 8
  %"arg:exponent" = alloca i64, align 8
  store i64 %exponent, ptr %"arg:exponent", align 8
  br label %if_branch

if_branch:                                        ; preds = %entry
  %0 = load i64, ptr %"arg:exponent", align 8
  %1 = icmp eq i64 %0, 1
  %2 = icmp eq i1 %1, true
  %3 = load i64, ptr %"arg:base", align 8
  br i1 %2, label %merge_branch, label %else_branch

merge_branch:                                     ; preds = %else_branch, %if_branch
  %4 = phi i64 [ %3, %if_branch ], [ %10, %else_branch ]
  ret i64 %4

else_branch:                                      ; preds = %if_branch
  %5 = load i64, ptr %"arg:base", align 8
  %6 = load i64, ptr %"arg:base", align 8
  %7 = load i64, ptr %"arg:exponent", align 8
  %8 = sub i64 %7, 1
  %9 = call i64 @exp(i64 %6, i64 %8, {} undef)
  %10 = mul i64 %5, %9
  br label %merge_branch
}

define i64 @main({} %closure-env) {
entry:
  %0 = call i64 @fib(i64 10, {} undef)
  %1 = call i64 (ptr, ...) @printf(ptr @0, i64 %0)
  %i1 = alloca i64, align 8
  store i64 %1, ptr %i1, align 8
  %2 = call i64 @fac(i64 10, {} undef)
  %3 = call i64 (ptr, ...) @printf(ptr @1, i64 %2)
  %i2 = alloca i64, align 8
  store i64 %3, ptr %i2, align 8
  %4 = call i64 @exp(i64 2, i64 10, {} undef)
  %5 = call i64 (ptr, ...) @printf(ptr @2, i64 %4)
  %i3 = alloca i64, align 8
  store i64 %5, ptr %i3, align 8
  ret i64 0
}
