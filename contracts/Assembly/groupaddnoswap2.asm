PUSH 0x4
CALLDATALOAD
PUSH 0x24
CALLDATALOAD
PUSH 0x44
CALLDATALOAD
PUSH 0x64
CALLDATALOAD
PUSH 0x84
CALLDATALOAD
PUSH 0xA4
CALLDATALOAD
PUSH 0xC4
CALLDATALOAD
PUSH 0xE4
CALLDATALOAD
PUSH 0x0
EQ
PC
PUSH 0x19
ADD
JUMPI
POP
POP
POP
PUSH 0x60
MSTORE
PUSH 0x40
MSTORE
PUSH 0x20
MSTORE
PUSH 0x0
MSTORE
PUSH 0x80
PUSH 0x0
RETURN
JUMPDEST :NOTINFINITYA
SWAP3
PUSH 0x0
EQ
PC
PUSH 0x18
ADD
JUMPI
PUSH 0x0 
PUSH 0x60
MSTORE
PUSH 0x20
MSTORE
PUSH 0x0
MSTORE
PUSH 0x40
MSTORE
PUSH 0x80
PUSH 0x0
RETURN
JUMPDEST :NOTINIFINITYB
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
DUP4
DUP1
MULMOD
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
DUP2
DUP9
MULMOD
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
DUP7
DUP1
MULMOD
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
DUP3
DUP8
MULMOD
DUP4
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
SUB
ADDMOD
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
DUP2
DUP1
MULMOD
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
DUP2
DUP4
MULMOD
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
DUP8
DUP12
MULMOD
DUP13
MULMOD
SWAP6
POP
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
DUP7
DUP11
MULMOD
DUP13
MULMOD s2
DUP8
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
SUB
ADDMOD
DUP1
DUP1
SWAP6
POP
DUP5
ISZERO
ISZERO
PC
PUSH 0x60
ADD
JUMPI
DUP6
ISZERO
PC
PUSH 0x1E
ADD
JUMPI
PUSH 0x0
PUSH 0x0
MSTORE
PUSH 0x0
PUSH 0x20
MSTORE
PUSH 0x0
PUSH 0x40
MSTORE
PUSH 0x1
PUSH 0x60
MSTORE
PUSH 0x80
PUSH 0x0
RETURN
JUMPDEST :iequalzero
DUP10
PUSH 0x4
MSTORE
DUP9
PUSH 0x24
MSTORE
DUP11
PUSH 0x44
MSTORE
PUSH 0x0
PUSH 0x64
MSTORE
PUSH 0x80
PUSH 0x0
PUSH 0x84
PUSH 0x0
PUSH 0x0
PUSH 0xff6a1c205b133b8ef5e5b4be40fde14772e54277
PUSH 0x32
GAS
SUB
CALLCODE
PUSH 0x80
PUSH 0x0
RETURN
JUMPDEST :h
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
SWAP2
MULMOD
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
DUP6
DUP13
MULMOD
DUP13
MULMOD
PUSH 0x40
MSTORE
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
DUP13
DUP11
MULMOD
DUP1
SWAP11
POP
PUSH 0x2
MULMOD
DUP8
ADDMOD
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
SUB
DUP6
ADDMOD
DUP1
PUSH 0x0
MSTORE
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
SUB
DUP8
ADDMOD
DUP8
MULMOD
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
DUP5
DUP11
MULMOD
PUSH32 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
SUB
ADDMOD
PUSH 0x20
MSTORE
PUSH 0x80
PUSH 0x0
RETURN


