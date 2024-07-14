; ModuleID = 'kebab'
source_filename = "kebab"

@i1 = global i64 1
@i2 = global i64 6
@i3 = global float mul (float 2.500000e+00, float 0x4008CCCCC0000000)
@i4 = global float 0x4003A2E8C0000000
@i5 = global i64 -10

declare i32 @printf(ptr, ...)

define i32 @main(i32 %0, ptr %1) {
entry:
  ret i32 0
}
