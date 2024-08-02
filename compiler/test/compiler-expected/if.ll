; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @main({} %__closure_env) {
entry:
  br label %if_branch

if_branch:                                        ; preds = %entry
  %local = alloca i64, align 8
  store i64 420, ptr %local, align 8
  %0 = load i64, ptr %local, align 8
  %1 = add i64 %0, 1
  br i1 false, label %merge_branch, label %elif_branch

merge_branch:                                     ; preds = %else_branch, %elif_branch, %if_branch
  %2 = phi i64 [ %1, %if_branch ], [ %7, %elif_branch ], [ %9, %else_branch ]
  %i1 = alloca i64, align 8
  store i64 %2, ptr %i1, align 8
  %3 = load i64, ptr %i1, align 8
  %4 = call i64 (ptr, ...) @printf(ptr @0, i64 %3)
  %i2 = alloca i64, align 8
  store i64 %4, ptr %i2, align 8
  %5 = load i64, ptr %i2, align 8
  ret i64 0

elif_branch:                                      ; preds = %if_branch
  %local1 = alloca i64, align 8
  store i64 1024, ptr %local1, align 8
  %6 = load i64, ptr %local1, align 8
  %7 = add i64 %6, 2
  br i1 true, label %merge_branch, label %else_branch

else_branch:                                      ; preds = %elif_branch
  %local2 = alloca i64, align 8
  store i64 69, ptr %local2, align 8
  %8 = load i64, ptr %local2, align 8
  %9 = add i64 %8, 42
  br label %merge_branch
}
