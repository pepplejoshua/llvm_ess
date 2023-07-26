; ModuleID = 'llvm_ess'
source_filename = "llvm_ess"

define i32 @foo(i32 %a, i32 %b) {
entry:
  %multmp = mul i32 %a, 16
  %cmptmp = icmp ult i32 %a, 100
  %ze_cmptmp = zext i1 %cmptmp to i32
  %if_cond = icmp ne i32 %ze_cmptmp, 0
  br i1 %if_cond, label %then, label %else

then:                                             ; preds = %entry
  %thenaddtmp = add i32 %a, 1
  br label %ifcont

else:                                             ; preds = %entry
  %elseaddtmp = add i32 %a, 2
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %iftmp = phi i32 [ %thenaddtmp, %then ], [ %elseaddtmp, %else ]
  ret i32 %iftmp
}