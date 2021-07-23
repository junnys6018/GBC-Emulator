"""
Generates a test rom with every opcode once
"""
import sys

# 0 == illegal opcode
opcode_lengths = [
    1, 3, 1, 1, 1, 1, 2, 1, 3, 1, 1, 1, 1, 1, 2, 1,
    1, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1,
    2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1,
    2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 3, 3, 3, 1, 2, 1, 1, 1, 3, 1, 3, 3, 2, 1,
    1, 1, 3, 0, 3, 1, 2, 1, 1, 1, 3, 0, 3, 0, 2, 1,
    2, 1, 1, 0, 0, 1, 2, 1, 2, 1, 3, 0, 0, 0, 2, 1,
    2, 1, 1, 1, 0, 1, 2, 1, 2, 1, 3, 1, 0, 0, 2, 1
]

relative_jump_instructions = [0x18, 0x20, 0x28, 0x30, 0x38]
absolute_jump_instructions = [
    0xC2, 0xC3, 0xCA, 0xD2, 0xDA, 0xE9,            # jumps
    0xC4, 0xCC, 0xCD, 0xD4, 0xDC,                  # calls
    0xC0, 0xC8, 0xC9, 0xD0, 0xD8, 0xD9,            # returns
    0xC7, 0xCF, 0xD7, 0xDF, 0xE7, 0xEF, 0xF7, 0xFF # restart
]

def insert(src, base, dest):
    for (i, v) in enumerate(dest):
        src[base + i] = v

pad = bytearray([0] * 0x100)
header = bytearray([0] * 0x50)

# nop
# jp 0150
jmp = [0x00, 0xC3, 0x50, 0x01]
insert(header, 0, jmp)

nintendo_logo = [
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
]
insert(header, 4, nintendo_logo)

# gbc flag
header[0x43] = 0x80

body_len = 1024 * 32 - len(pad) - len(header)
body = bytearray([0] * body_len)

offset = 0
# all non branching opcodes
for opcode in range(0, 256):
    size = opcode_lengths[opcode]
    if size != 0 and opcode not in relative_jump_instructions and opcode not in absolute_jump_instructions:
        body[offset] = opcode
        offset += 1
        for _ in range(0, size - 1):
            body[offset] = 0xCA
            offset += 1

# cb opcodes
for opcode in range(0, 256):
    body[offset] = 0xCB
    body[offset + 1] = opcode
    offset += 2

rom = pad + header + body
open(sys.argv[1], 'wb').write(rom)