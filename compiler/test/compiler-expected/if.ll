; ModuleID = 'kebab'
source_filename = "kebab"

@0 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1

declare i64 @printf(ptr, ...)

define i64 @main({} %0) {
entry:
  br label %if_branch

if_branch:                                        ; preds = %entry
  %local = alloca i64, align 8
  store i64 420, ptr %local, align 8
  %1 = load i64, ptr %local, align 8
  %2 = add i64 %1, 1
  br i1 false, label %merge_branch, label %elif_branch

merge_branch:                                     ; preds = %else_branch, %elif_branch, %if_branch
  %3 = phi i64 [ %2, %if_branch ], [ %8, %elif_branch ], [ %10, %else_branch ]
  %i1 = alloca i64, align 8
  store i64 %3, ptr %i1, align 8
  %4 = load i64, ptr %i1, align 8
  %5 = call i64 (ptr, ...) @printf(ptr @0, i64 %4)
  %i2 = alloca i64, align 8
  store i64 %5, ptr %i2, align 8
  %6 = load i64, ptr %i2, align 8
  ret i64 0

elif_branch:                                      ; preds = %if_branch
  %local1 = alloca i64, align 8
  store i64 1024, ptr %local1, align 8
  %7 = load i64, ptr %local1, align 8
  %8 = add i64 %7, 2
  br i1 true, label %merge_branch, label %else_branch

else_branch:                                      ; preds = %elif_branch
  %local2 = alloca i64, align 8
  store i64 69, ptr %local2, align 8
  %9 = load i64, ptr %local2, align 8
  %10 = add i64 %9, 42
  br label %merge_branch
}
