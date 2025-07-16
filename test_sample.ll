; Sample LLVM IR for IRGraph backend testing
define i32 @simple_add(i32 %a, i32 %b) {
entry:
  %result = add i32 %a, %b
  ret i32 %result
}

define i32 @complex_function(i32 %x, i32 %y) {
entry:
  %tmp1 = alloca i32
  store i32 %x, i32* %tmp1
  %tmp2 = alloca i32
  store i32 %y, i32* %tmp2
  
  %val1 = load i32, i32* %tmp1
  %val2 = load i32, i32* %tmp2
  
  %sum = add i32 %val1, %val2
  %product = mul i32 %sum, 2
  %quotient = udiv i32 %product, 3
  
  %cmp = icmp eq i32 %quotient, 10
  br i1 %cmp, label %then, label %else

then:
  %result1 = add i32 %quotient, 100
  br label %end

else:
  %result2 = sub i32 %quotient, 5
  br label %end

end:
  %final = phi i32 [ %result1, %then ], [ %result2, %else ]
  ret i32 %final
}