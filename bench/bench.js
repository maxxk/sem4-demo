function b0(a) { return a + 1; }
function b1(a) { return b0(b0(b0(b0(a)))); }
function b2(a) { return b1(b1(b1(b1(a)))); }
function b3(a) { return b2(b2(b2(b2(a)))); }
function b4(a) { return b3(b3(b3(b3(a)))); }
function b5(a) { return b4(b4(b4(b4(a)))); }
function b6(a) { return b5(b5(b5(b5(a)))); }
function b7(a) { return b6(b6(b6(b6(a)))); }
function b8(a) { return b7(b7(b7(b7(a)))); }
function b9(a) { return b8(b8(b8(b8(a)))); }
function b10(a) { return b9(b9(b9(b9(a)))); }
function b11(a) { return b10(b10(b10(b10(a)))); }
function b12(a) { return b11(b11(b11(b11(a)))); }
function b13(a) { return b12(b12(b12(b12(a)))); }

console.log(b13(0));
