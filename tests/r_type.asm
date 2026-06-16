add  x1, x2, x3      # x1 = x2 + x3
sub  x1, x2, x3      # x1 = x2 - x3
xor  x1, x2, x3      # x1 = x2 ^ x3
or   x1, x2, x3      # x1 = x2 | x3
and  x1, x2, x3      # x1 = x2 & x3

sll  x1, x2, x3      # x1 = x2 << (x3 & 0x1F)
srl  x1, x2, x3      # logical right shift
sra  x1, x2, x3      # arithmetic right shift

slt  x1, x2, x3      # x1 = (x2 < x3) ? 1 : 0
sltu x1, x2, x3      # unsigned comparison
