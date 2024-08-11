@0 = private unnamed_addr constant [5 x i8] c"%ld\0A\00", align 1

declare i64 @printf(ptr, ...)

define ptr @returns-one-factory({} %closure-env) {
entry:
  ret ptr @__anonymous_function
}

define i64 @__anonymous_function({} %closure-env) {
entry:
  ret i64 1
}

define i64 @one-factory({} %closure-env) {
entry:
  %0 = call ptr @returns-one-factory({} zeroinitializer)
  %1 = call i64 %0({} zeroinitializer)
  ret i64 %1
}

define i64 @main({} %closure-env) {
entry:
  %0 = call i64 @one-factory({} zeroinitializer)
  %1 = call i64 @printf(ptr @0, i64 %0)
  
  ret i64 0
}
