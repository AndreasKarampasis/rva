addi  x1, x2, 10     # x1 = x2 + 10
xori  x1, x2, 0xFF   # x1 = x2 ^ 0xFF
ori   x1, x2, 0xF0   # x1 = x2 | 0xF0
andi  x1, x2, 0x0F   # x1 = x2 & 0x0F

slli  x1, x2, 3      # x1 = x2 << 3
srli  x1, x2, 3      # logical shift right
srai  x1, x2, 3      # arithmetic shift right

slti  x1, x2, 100    # signed compare
sltiu x1, x2, 100    # unsigned compare
