; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @main({} %closure-env) {
entry:
  %unused-local = alloca i64, align 8
  store i64 0, ptr %unused-local, align 8
  %unused-local2 = alloca i64, align 8
  store i64 2, ptr %unused-local2, align 8
  %0 = insertvalue { ptr, ptr } undef, ptr %unused-local, 0
  %closure-arg = insertvalue { ptr, ptr } %0, ptr %unused-local2, 1
  %1 = call i64 @exp-tail(i64 2, i64 5, { ptr, ptr } %closure-arg)
  %2 = call i64 (ptr, ...) @printf(ptr @0, i64 %1)
  ret i64 0
}

define i64 @exp-tail(i64 %base, i64 %exponent, { ptr, ptr } %closure-env) {
entry:
  %"closure-env:unused-local" = extractvalue { ptr, ptr } %closure-env, 0
  %"closure-env:unused-local-ptr" = alloca ptr, align 8
  store ptr %"closure-env:unused-local", ptr %"closure-env:unused-local-ptr", align 8
  %0 = load ptr, ptr %"closure-env:unused-local-ptr", align 8
  %"closure-env:unused-local2" = extractvalue { ptr, ptr } %closure-env, 1
  %"closure-env:unused-local2-ptr" = alloca ptr, align 8
  store ptr %"closure-env:unused-local2", ptr %"closure-env:unused-local2-ptr", align 8
  %1 = load ptr, ptr %"closure-env:unused-local2-ptr", align 8
  %"arg:base" = alloca i64, align 8
  store i64 %base, ptr %"arg:base", align 8
  %"arg:exponent" = alloca i64, align 8
  store i64 %exponent, ptr %"arg:exponent", align 8
  %2 = load i64, ptr %"arg:exponent", align 8
  %3 = insertvalue { ptr, ptr, ptr, ptr } undef, ptr %0, 0
  %4 = insertvalue { ptr, ptr, ptr, ptr } %3, ptr %1, 1
  %5 = insertvalue { ptr, ptr, ptr, ptr } %4, ptr %"arg:base", 2
  %closure-arg = insertvalue { ptr, ptr, ptr, ptr } %5, ptr %"arg:exponent", 3
  %6 = call i64 @exp-tail-impl(i64 %2, i64 1, { ptr, ptr, ptr, ptr } %closure-arg)
  ret i64 %6
}

define i64 @exp-tail-impl(i64 %exponent, i64 %acc, { ptr, ptr, ptr, ptr } %closure-env) {
entry:
  %"closure-env:unused-local" = extractvalue { ptr, ptr, ptr, ptr } %closure-env, 0
  %"closure-env:unused-local-ptr" = alloca ptr, align 8
  store ptr %"closure-env:unused-local", ptr %"closure-env:unused-local-ptr", align 8
  %0 = load ptr, ptr %"closure-env:unused-local-ptr", align 8
  %"closure-env:unused-local2" = extractvalue { ptr, ptr, ptr, ptr } %closure-env, 1
  %"closure-env:unused-local2-ptr" = alloca ptr, align 8
  store ptr %"closure-env:unused-local2", ptr %"closure-env:unused-local2-ptr", align 8
  %1 = load ptr, ptr %"closure-env:unused-local2-ptr", align 8
  %"closure-env:base" = extractvalue { ptr, ptr, ptr, ptr } %closure-env, 2
  %"closure-env:base-ptr" = alloca ptr, align 8
  store ptr %"closure-env:base", ptr %"closure-env:base-ptr", align 8
  %2 = load ptr, ptr %"closure-env:base-ptr", align 8
  %"closure-env:exponent" = extractvalue { ptr, ptr, ptr, ptr } %closure-env, 3
  %"closure-env:exponent-ptr" = alloca ptr, align 8
  store ptr %"closure-env:exponent", ptr %"closure-env:exponent-ptr", align 8
  %3 = load ptr, ptr %"closure-env:exponent-ptr", align 8
  %"arg:exponent" = alloca i64, align 8
  store i64 %exponent, ptr %"arg:exponent", align 8
  %"arg:acc" = alloca i64, align 8
  store i64 %acc, ptr %"arg:acc", align 8
  br label %if_branch

if_branch:                                        ; preds = %entry
  %4 = load i64, ptr %"arg:exponent", align 8
  %5 = icmp eq i64 %4, 0
  %6 = icmp eq i1 %5, true
  %7 = load i64, ptr %"arg:acc", align 8
  br i1 %6, label %merge_branch, label %else_branch

merge_branch:                                     ; preds = %else_branch, %if_branch
  %8 = phi i64 [ %7, %if_branch ], [ %17, %else_branch ]
  ret i64 %8

else_branch:                                      ; preds = %if_branch
  %9 = load i64, ptr %"arg:exponent", align 8
  %10 = sub i64 %9, 1
  %11 = load i64, ptr %"arg:acc", align 8
  %12 = load i64, ptr %2, align 8
  %13 = mul i64 %11, %12
  %14 = insertvalue { ptr, ptr, ptr, ptr } undef, ptr %0, 0
  %15 = insertvalue { ptr, ptr, ptr, ptr } %14, ptr %1, 1
  %16 = insertvalue { ptr, ptr, ptr, ptr } %15, ptr %2, 2
  %closure-arg = insertvalue { ptr, ptr, ptr, ptr } %16, ptr %"arg:exponent", 3
  %17 = call i64 @exp-tail-impl(i64 %10, i64 %13, { ptr, ptr, ptr, ptr } %closure-arg)
  br label %merge_branch
}
