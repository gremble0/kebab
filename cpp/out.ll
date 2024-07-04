; ModuleID = 'kebab'
source_filename = "kebab"

@v = global ptr @printf

declare i32 @printf(ptr, ...)

define i32 @main(i32 %0, ptr %1) {
entry:
  ret i32 0
}
