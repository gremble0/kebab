; ModuleID = 'kebab'
source_filename = "kebab"

declare i64 @printf(ptr, ...)

declare ptr @malloc(i64)

define i64 @main({} %closure-env) {
entry:
  ret i64 0
}

define i64 @local({} %closure-env) {
entry:
  %my-int = alloca i64, align 8
  store i64 9, ptr %my-int, align 8
  %closure-arg = insertvalue { ptr } undef, ptr %my-int, 0
  %0 = call i64 @local-to-local({ ptr } %closure-arg)
  %return = alloca i64, align 8
  store i64 %0, ptr %return, align 8
  %1 = load i64, ptr %return, align 8
  ret i64 %1
}

define i64 @local-to-local({ ptr } %closure-env) {
entry:
  %"closure-env:my-int" = extractvalue { ptr } %closure-env, 0
  %"closure-env:my-int-ptr" = alloca ptr, align 8
  store ptr %"closure-env:my-int", ptr %"closure-env:my-int-ptr", align 8
  %0 = load ptr, ptr %"closure-env:my-int-ptr", align 8
  %1 = load i64, ptr %0, align 8
  ret i64 %1
}
