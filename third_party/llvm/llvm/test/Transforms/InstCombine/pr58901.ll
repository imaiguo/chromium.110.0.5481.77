; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -passes=instcombine -S | FileCheck %s

define i32* @f1([6 x i32]* %arg, i64 %arg1) {
; CHECK-LABEL: @f1(
; CHECK-NEXT:    [[TMP1:%.*]] = getelementptr [6 x i32], [6 x i32]* [[ARG:%.*]], i64 3, i64 [[ARG1:%.*]]
; CHECK-NEXT:    ret i32* [[TMP1]]
;
  %1 = getelementptr [6 x i32], [6 x i32]* %arg, i64 3
  %2 = getelementptr [6 x i32], [6 x i32]* %1, i64 0, i64 %arg1
  ret i32* %2
}

define i32* @f2([6 x i32]* %arg, i64 %arg1) {
; CHECK-LABEL: @f2(
; CHECK-NEXT:    [[TMP1:%.*]] = getelementptr [6 x i32], [6 x i32]* [[ARG:%.*]], i64 3
; CHECK-NEXT:    [[TMP2:%.*]] = getelementptr [6 x i32], [6 x i32]* [[TMP1]], i64 [[ARG1:%.*]], i64 [[ARG1]]
; CHECK-NEXT:    ret i32* [[TMP2]]
;
  %1 = getelementptr [6 x i32], [6 x i32]* %arg, i64 3
  %2 = getelementptr [6 x i32], [6 x i32]* %1, i64 %arg1, i64 %arg1
  ret i32* %2
}
