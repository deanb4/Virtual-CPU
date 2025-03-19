# CPU Design

## ISA/Encoding
* 32 bits

# R-Type
|Opcode| |rs| |rt| |rd| |shamt| |opcode| Bits: (6,5,5,5,5,6)

# I-Type
|Opcode| |rs| |rd| |Immediate| Bits: (6,5,5,16)

# J type
|Opcode| |Immediate| Bits: (6,26)

## Class Layout
* use enum and array for resgisters for O(1) access and readability
* enum for syscalls starting from 1 

# Class Functions:
* default Constructor
* Constructor( memory, sp,pc,gp, halted = false, register count, set up syscalls,flags)
* LoadProgram(takes an array of instructions)
* fetch(get instruction from memory and pass to decodeExecute)
* decodeExecute(takes the current line of instructions and executes it)
* print syscall, read syscall etc, exit syscall etc....

## work flow of instruction execution
* Load program into memory
* Fetch instruction from memory
* decode instruction (control Unit figures out Opcode and Sets up CPU to execute)
* Execute given instruction
* Repeat until reached halt instruction

# Registers
## General-Purpose Registers (32 Registers)
These registers are denoted as `$0` to `$31` and are used for various purposes during program execution. Some registers have specific purposes, but many are used by the programmer for general-purpose operations.

| Register | Name            | Description                                                             |
|----------|-----------------|-------------------------------------------------------------------------|
| `$0`     | `$zero`         | Constant zero (always holds 0).                                         |
| `$1`     | `$at`           | Assembler temporary (used by assembler for intermediate values).        |
| `$2`-$3` | `$v0`-$v1`      | Function return values (used to return values from functions).          |
| `$4`-$7` | `$a0`-$a3`      | Function arguments (up to 4 arguments can be passed via these registers).|
| `$8`-$15`| `$t0`-$t7`      | Temporary registers (used for intermediate results, not preserved).     |
| `$16`-$23`| `$s0`-$s7`     | Saved registers (preserved across function calls).                      |
| `$24`-$25`| `$t8`-$t9`     | Temporary registers (used for intermediate results, not preserved).     |
| `$26`-$27`| `$k0`-$k1`     | Kernel registers (used by the operating system, not usually modified by programs).|
| `$28`    | `$gp`           | Global pointer (used to access global variables).                       |
| `$29`    | `$sp`           | Stack pointer (points to the top of the stack).                         |
| `$30`    | `$fp`           | Frame pointer (used to point to the current stack frame).               |
| `$31`    | `$ra`           | Return address (used to store the return address for function calls).   |

## Special Registers
These registers have specific functions for controlling the processor and managing system operations.

| Register   | Name            | Description                                                            |
|------------|-----------------|------------------------------------------------------------------------|
| `$hi`      | High            | Used to hold the high part of a result from multiplication or division.|
| `$lo`      | Low             | Used to hold the low part of a result from multiplication or division. |
| `$pc`      | Program Counter | Holds the address of the next instruction to be executed.              |
| `$status`  | Status Register | Contains the status of the processor (e.g., interrupt enable flags).   |
| `$cause`   | Cause Register  | Indicates the cause of an exception or interrupt.                      |
| `$epc`     | Exception Program Counter | Holds the address of the instruction that caused an exception.  |
| `$badvaddr`| Bad Virtual Address | Holds the virtual address causing a memory exception.               |

## Floating Point Registers  
For floating-point operations, MIPS has a separate set of 32 registers (denoted `$f0` to `$f31`).

| Register | Name            | Description                                |
|----------|-----------------|--------------------------------------------|
| `$f0`-$f31` | Floating-Point Registers | Used for storing floating-point values. |

# Control Registers  
Control registers are used for various control operations, such as handling exceptions, interrupts, and memory management.

- **$status**: Indicates the status of the CPU, including interrupt enable flags.
- **$cause**: Holds the cause of exceptions.
- **$epc**: Stores the address of the instruction that caused an exception.
- **$badvaddr**: Holds the address of the bad memory access during exceptions.

## Flags 

### 1. **Zero Flag (Z)**
   - **Purpose**: Indicates if the result of an arithmetic or logical operation is zero.
   - **Usage**: This flag is essential for conditional branches, such as **BEQ** (branch if equal). It allows checking if the result of an operation was zero to make decisions about branching.

### 2. **Negative Flag (N)**
   - **Purpose**: Set when the result of an operation is negative.
   - **Usage**: Used in conditional branches like **BLT** (branch if less than) and **BGE** (branch if greater than or equal). It helps determine if the result of an operation is negative.

### 3. **Overflow Flag (V)**
   - **Purpose**: Indicates whether an overflow occurred in an arithmetic operation.
   - **Usage**: This flag helps detect overflows in operations like addition and subtraction. For example, if adding two large positive numbers results in a negative number, it signals an overflow.

### 4. **Carry Flag (C)**
   - **Purpose**: Indicates whether a carry-out occurred in an addition or a borrow occurred in a subtraction.
   - **Usage**: The carry flag is useful for unsigned arithmetic operations, such as multi-precision arithmetic, where the result exceeds the maximum value of the register size.

### 5. **Exception Flag**
   - **Purpose**: Indicates when an exception or error occurs, such as divide-by-zero or invalid instruction.
   - **Usage**: This flag triggers an interrupt or exception handler to manage errors during program execution.


## Notes on implementation

## Instruction Set
# add   
    add $1,$2,$3 $1 = $2 + $3 3 operands 
# subtract
  sub $1,$2,$3 $1 = $2 – $3 3 operands 
# add immediate 
    addi $1,$2,100 $1 = $2 + 100 + constant 
# multiply   
    mult $2,$3 Hi, Lo = $2 x $3 64-bit signed product 
# divide
   div $2,$3  Lo = $2 ÷ $3, Lo = quotient,  
      Hi = $2 mod $3       Hi = remainder   
# Move from Hi
    mfhi $1  $1 = Hi  Used to get copy of Hi 
# Move from Lo
    mflo $1  $1 = Lo  Used to get copy of Lo

## Logical Instructions
# and   
    and $1,$2,$3 $1 = $2 & $3 Bitwise AND 
# or   
    or $1,$2,$3 $1 = $2 | $3 Bitwise OR 
# xor
    xor $1,$2,$3 $1 = $2 ⊕ $3 Bitwise XOR 
# nor
  nor $1,$2,$3 $1 = ~($2 | $3) Bitwise NOR 
# and immediate
 andi $1,$2,10 $1 = $2 & 10 Bitwise AND reg, const 
# or immediate 
    ori $1,$2,10 $1 = $2 | 10 Bitwise OR reg, const 
# xor immediate
  xori $1, $2,10  $1 = ~$2 &~10 Bitwise XOR reg, const 
# shift left logical
 sll $1,$2,10 $1 = $2 << 10 Shift left by constant 
# shift right logical
 srl $1,$2,10 $1 = $2 >> 10 Shift right by constant 

## Memory operations
# lb   
    lb $t0, offset($t1)      # Load Byte 
# lh   
    lh $t0, offset($t1)      # Load Halfword 
# lw   
    lw $t0, offset($t1)      # Load Word 
# sb   
    sb $t0, offset($t1)      # Store Byte 
# sh   
    sh $t0, offset($t1)      # Store Halfword 
# sw   
    sw $t0, offset($t1)      # Store Word 
# sc   
    sc $t0, offset($t1)      # Store Conditional 

# Branch Instructions

# Unconditional Jump Instructions (might not implement all of them)
j target          # Jump to target address
jr $rs           # Jump to address in register $rs (used for function returns)
jal target       # Jump and link (stores return address in $ra)
jalr $rd, $rs    # Jump and link register (stores return address in $rd, jumps to $rs)

# Conditional Branch Instructions ((might not implement all of them))
beq $rs, $rt, offset   # Branch if $rs == $rt
bne $rs, $rt, offset   # Branch if $rs != $rt
bgez $rs, offset       # Branch if $rs >= 0
bgtz $rs, offset       # Branch if $rs > 0
blez $rs, offset       # Branch if $rs <= 0
bltz $rs, offset       # Branch if $rs < 0

# Pseudo-Instructions for Branching
b target              # Unconditional branch (beq $zero, $zero, target)
bgt $rs, $rt, target  # Branch if $rs > $rt (uses slt and bne)
bge $rs, $rt, target  # Branch if $rs >= $rt (uses slt and beq)
blt $rs, $rt, target  # Branch if $rs < $rt (uses slt and bne)
ble $rs, $rt, target  # Branch if $rs <= $rt (uses slt and beq)

# Function Return and Exception Handling
jr $ra      # Return from function (jump to $ra)
eret        # Return from exception handler

# Syscalls
# I/O Operations 
1  - print_int        # Print integer in $a0
2  - print_float      # Print float in $f12
3  - print_double     # Print double in $f12
4  - print_string     # Print string (address in $a0)
5  - read_int         # Read integer, result in $v0
6  - read_float       # Read float, result in $f0
7  - read_double      # Read double, result in $f0
8  - read_string      # Read string (buffer in $a0, length in $a1)
# Memory Allocation
9  - sbrk             # Allocate heap memory (size in $a0, address returned in $v0)
# File Operations
10 - exit             # Exit program
11 - print_character  # Print character in $a0
12 - read_character   # Read character, result in $v0
13 - open_file        # Open file (filename in $a0, flags in $a1, mode in $a2, returns fd in $v0)
14 - read_file        # Read file (fd in $a0, buffer in $a1, size in $a2, bytes read in $v0)
15 - write_file       # Write file (fd in $a0, buffer in $a1, size in $a2, bytes written in $v0)
16 - close_file       # Close file (fd in $a0)
17 - exit2            # Exit with status (status in $a0)

# Syscall Table

| Syscall # | Description       | `$a0`        | `$a1`        | `$a2`        | `$f12` (Float/Double) | Return (`$v0`) |
|-----------|------------------|--------------|--------------|--------------|------------------------|----------------|
| **1**     | Print Integer     | Integer to print | - | - | - | None |
| **2**     | Print Float       | - | - | - | Float in `$f12` | None |
| **3**     | Print Double      | - | - | - | Double in `$f12` | None |
| **4**     | Print String      | Address of string | - | - | - | None |
| **5**     | Read Integer      | - | - | - | - | Integer read |
| **6**     | Read Float        | - | - | - | - | Float read in `$f0` |
| **7**     | Read Double       | - | - | - | - | Double read in `$f0` |
| **8**     | Read String       | Buffer address | Max length | - | - | None (buffer modified) |
| **9**     | Exit Program      | - | - | - | - | None |
| **10**    | Exit with Status  | Exit status | - | - | - | None |
| **13**    | Open File         | Filename address | Flags (0 = read, 1 = write, etc.) | Mode (ignored for read) | - | File descriptor (-1 on failure) |
| **14**    | Read File         | File descriptor | Buffer address | Number of bytes | - | Bytes read |
| **15**    | Write File        | File descriptor | Buffer address | Number of bytes | - | Bytes written |
| **16**    | Close File        | File descriptor | - | - | - | None |
| **17**    | Exit with Status  | Exit status | - | - | - | None |


## Memory Layout
### Total Memory: 1GB (1,073,741,824 bytes)
## Text Segment (code):
* 0x00000000 - 0x000FFFFF (1MB)
* 0 - 1,048,575

## Data Segment
### Uninitialized Data
* 0x00100000 - 0x001FFFFF (1MB)
* 1,048,576 - 2,097,151

### Initialized Data
* x00200000 - 0x002FFFFF (1MB)
* 2,097,152 - 3,145,727

### Heap (Dynamic Memory)
* 0x00300000 - 0x07FFFFFF (125MB)
* 3,145,728 - 134,217,727

### Stack
* 0x3FFFFFFF and grows downward (256MB)
* 1,073,741,823

### Reserved Space (unused)
* 0x08000000 - 0x3FFFFFFF (640MB)
* 134,217,728 - 1,073,741,823

### Kernel Text Segment (unused)
* 0x80000000 - 0x8FFFFFFF
*2,147,483,648 - 2,268,435,455

### Kernel Data Segment (unused) 
* 0x90000000 - 0x9FFFFFFF
* 2,368,435,456 - 2,499,385,855

### Memory Mapped I/O
* 0xFFFF0000 - 0xFFFFFFFF
* 4,294,967,296 - 4,294,967,295

## Memory Layout New
0x00000000 - 0x000FFFFF : TEXT_MEMORY        (1 MB)           // Code Section
0x00010000 - 0x00400000 : GLOBAL_DATA_MEMORY  (4 MB)           // Global Data Section (initialized data)
0x00400000 - 0x20000000 : HEAP_MEMORY         (512 MB)         // Heap Section (dynamic memory)
0x3F000000 - 0x3FFFFFFF : STACK_MEMORY        (512 MB)         // Stack Section (grows downwards)
0x7F000000 - 0x80000000 : KERNEL_MEMORY       (512 MB)         // Kernel Reserved Area (high memory)
