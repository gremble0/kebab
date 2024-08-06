; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1
@1 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1
@2 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @fib(i64 %n, {} %closure-env) {
entry:
  br label %if_branch

if_branch:                                        ; preds = %entry
  %0 = icmp ult i64 %n, 2
  %1 = icmp eq i1 %0, true
  br i1 %1, label %merge_branch, label %else_branch

merge_branch:                                     ; preds = %else_branch, %if_branch
  %2 = phi i64 [ %n, %if_branch ], [ %7, %else_branch ]
  ret i64 %2

else_branch:                                      ; preds = %if_branch
  %3 = sub i64 %n, 1
  %4 = call i64 @fib(i64 %3, {} undef)
  %5 = sub i64 %n, 2
  %6 = call i64 @fib(i64 %5, {} undef)
  %7 = add i64 %4, %6
  br label %merge_branch
}

define i64 @fac(i64 %n, {} %closure-env) {
entry:
  br label %if_branch

if_branch:                                        ; preds = %entry
  %0 = icmp ule i64 %n, 1
  %1 = icmp eq i1 %0, true
  br i1 %1, label %merge_branch, label %else_branch

merge_branch:                                     ; preds = %else_branch, %if_branch
  %2 = phi i64 [ %n, %if_branch ], [ %5, %else_branch ]
  ret i64 %2

else_branch:                                      ; preds = %if_branch
  %3 = sub i64 %n, 1
  %4 = call i64 @fac(i64 %3, {} undef)
  %5 = mul i64 %n, %4
  br label %merge_branch
}

define i64 @exp(i64 %base, i64 %exponent, {} %closure-env) {
entry:
  br label %if_branch

if_branch:                                        ; preds = %entry
  %0 = icmp eq i64 %exponent, 1
  %1 = icmp eq i1 %0, true
  br i1 %1, label %merge_branch, label %else_branch

merge_branch:                                     ; preds = %else_branch, %if_branch
  %2 = phi i64 [ %base, %if_branch ], [ %5, %else_branch ]
  ret i64 %2

else_branch:                                      ; preds = %if_branch
  %3 = sub i64 %exponent, 1
  %4 = call i64 @exp(i64 %base, i64 %3, {} undef)
  %5 = mul i64 %base, %4
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
