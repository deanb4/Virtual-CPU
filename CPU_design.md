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
jalr $rs, $rd    # Jump and link register (stores return address in $rd, jumps to $rs)

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

## *************Pipeline
1. IF (Instruction Fetch): Fetches instruction from memory.
2. ID (Instruction Decode & Register Read): Decodes the instruction and reads register values.
3. EX (Execute): Performs ALU operations and computes memory addresses.
4. MEM (Memory Access): Reads from or writes to memory.
5. WB (Write-Back): Writes the result to the destination register.

1. IF/ID Register (Between IF & ID)
   - Instruction (IR): The fetched instruction (32-bit).
   - PC+4: Address of the next instruction.

2. ID/EX Register (Between ID & EX)
   - Read Register Values (R1, R2): The values from the source registers.
   - Immediate (Imm): Sign-extended immediate value (for I-type instructions).
   - Control Signals: ALU operation, register destination, memory read/write, etc.
   - Destination Register (RD or RT): The register that will receive the result.

3. EX/MEM Register (Between EX & MEM)
   - ALU Result: Computed result (e.g., addition, logical operation, memory address).
   - Store Data (R2): Value to be stored (if it's a `sw` instruction).
   - Control Signals: Whether to read/write memory or write back to a register.
   - Destination Register: Register that will receive the result.

4. MEM/WB Register (Between MEM & WB)
   - ALU Result or Memory Data: The result from the ALU or the loaded memory value (if `lw`).
   - Destination Register: The register that will be updated.
   - Control Signals: Whether to write to the register file.


   We will execute these instructions through the pipeline, with the pipeline stages overlapping in each cycle.

### Program to Simulate 5-Stage Pipeline

```assembly
    add $t0, $t1, $t2
    sub $t3, $t4, $t5
    lw $t6, 0($t7)
    sw $t8, 4($t9)
    beq $t0, $t1, label
```
### **Execution Cycle Breakdown**

| Cycle | IF (Fetch)  | ID (Decode)  | EX (Execute)  | MEM (Memory)  | WB (Write-back) |
|-------|-------------|--------------|---------------|---------------|-----------------|
| 1     | ADD         |              |               |               |                 |
| 2     | SUB         | ADD          |               |               |                 |
| 3     | LW          | SUB          | ADD           |               |                 |
| 4     | SW          | LW           | SUB           | ADD           |                 |
| 5     | BEQ         | SW           | LW            | SUB           | ADD             |
| 6     |             | BEQ          | SW            | LW            | SUB             |

### **Explanation of Each Cycle:**

1. **Cycle 1**:
   - **IF**: Instruction `ADD $t0, $t1, $t2` is fetched.
   
2. **Cycle 2**:
   - **IF**: Instruction `SUB $t3, $t4, $t5` is fetched.
   - **ID**: Instruction `ADD` is decoded and the operands (`$t1`, `$t2`) are read.
   
3. **Cycle 3**:
   - **IF**: Instruction `LW $t6, 0($t7)` is fetched.
   - **ID**: Instruction `SUB` is decoded and the operands (`$t4`, `$t5`) are read.
   - **EX**: The `ADD` instruction is executed, and the result is computed.

4. **Cycle 4**:
   - **IF**: Instruction `SW $t8, 4($t9)` is fetched.
   - **ID**: Instruction `LW` is decoded and the address `0($t7)` is calculated.
   - **EX**: Instruction `SUB` is executed, and the result is computed.
   - **MEM**: `ADD` is completed, and the result is written back to the register file.

5. **Cycle 5**:
   - **IF**: Instruction `BEQ $t0, $t1, label` is fetched.
   - **ID**: Instruction `SW` is decoded and the address `4($t9)` is calculated.
   - **EX**: Instruction `LW` performs the memory read.
   - **MEM**: Instruction `SUB` performs memory operations.
   - **WB**: Instruction `ADD` writes the result back to `$t0`.

6. **Cycle 6** (and onward):
   - The pipeline continues by decoding and executing the next instructions, following the same pattern.

## Pipeline Hazards
Forwarding
Branch Prediction
stalls
data hazards
add display pipeline at every stage to debug method (display it in a graph layout)

## **********Error Handeling (Exceptions)
Memory errors: Add function that checks for valid memory to use before accessing memory at any point. 
Invalid instruction: Check before decoding if opcode is valid. If invalid put that instruction memory location in epc special register
change that on division by 0 and other exceptions I add that address to epc to display to user at what instruction program failed
Valid Syscall: Check that syscall val is valid if not return address of syscall inst to epc speical register
Check exception syscall usage 
** create function handleExcpetion() that is called at the end of every execute stage. if flags.excpetion is true then run the handle exception function
** go over each flag exception case and return correct throw error.

The following outlines the steps taken when an exception occurs in the CPU, (following the MIPS-like architecture)

1. **Exception Detected**:
   - During instruction execution, if an exception condition (such as overflow, division by zero, illegal instruction, etc.) is detected, normal execution is interrupted.

2. **Set Exception Flag**:
   - The `exceptionFlag` is set to `true`, signaling that an exception has occurred.
   - This flag is checked by the CPU to determine if an exception handler should be invoked.

3. **Save Instruction Address in EPC Register**:
   - The **EPC Register** (Exception Program Counter) is set to the address of the instruction that caused the exception.
   - This ensures the CPU knows where to resume execution once the exception is handled.

4. **Set the Cause Register**:
   - The **Cause Register** is set to a specific value, indicating the type of exception (e.g., overflow, division by zero).
   - The value in the Cause Register helps the CPU identify the specific exception type and take the appropriate action.

5. **Jump to Exception Handler**:
   - After setting the necessary flags and registers, control is transferred to an exception handler that will handle the exception appropriately.
   - The handler uses the value in the **Cause Register** to decide what action to take (e.g., terminating the program, retrying the operation).

6. **Handle the Exception**:
   - The exception handler inspects the **Cause Register** to identify the exception type and responds accordingly (e.g., error message, recovery routine).

7. **Return from Exception**:
   - After handling the exception, the CPU retrieves the address of the instruction that caused the exception from the **EPC Register**.
   - Execution resumes from the address stored in EPC, ensuring that the program continues after the exception.

### Table of Exception Codes (not all used)

| Exception Type                 | Cause Register Value |
|---------------------------------|----------------------|
| **Interrupt**                   | 0x00                 |
| **TLB Mod**                     | 0x01                 |
| **TLB Load**                    | 0x02                 |
| **TLB Store**                   | 0x03                 |
| **Address Error (Load/Store)**  | 0x04                 |
| **Bus Error**                   | 0x05                 |
| **System Call**                 | 0x08                 |
| **Break**                       | 0x09                 |
| **Reserved Instruction**        | 0x0A                 |
| **Coprocessor Unusable**        | 0x0B                 |
| **Overflow**                    | 0x0C                 |
| **Trap**                        | 0x0D                 |
| **Floating Point Exception**    | 0x10                 |
| **Virtualization Exception**    | 0x18                 |
| **Watchdog Timer**              | 0x20                 |
| **External Interrupt**          | 0x30                 |
| **Division by Zero**            | 0x04                 |
| **Invalid Instruction**         | 0x02                 |



## Cache 


## Add Performence metric capabilities (such as when using perf/time)

## Floating Point (maybe?)


** divide update 
div $s, $t         # Divide $s by $t, quotient in lo, remainder in hi
mflo $d            # Move quotient to $d
mfhi $r            # Move remainder to $r (optional)


# Control Unit
- **RegDst**: Selects destination register (0 = rt, 1 = rd).
- **ALUSrc**: Selects ALU input (0 = register, 1 = immediate).
- **MemToReg**: Determines if data comes from memory (1) or ALU (0).
- **RegWrite**: Enables writing to the register file.
- **MemRead**: Enables reading from memory.
- **MemWrite**: Enables writing to memory.
- **Branch**: Indicates a branch instruction.
- **Jump**: Indicates a jump instruction.
- **ALUOp**: Specifies the ALU operation.

| Opcode  | Instruction | RegDst | ALUSrc | MemToReg | RegWrite | MemRead | MemWrite | Branch | Jump | ALUOp |
|---------|------------|--------|--------|----------|----------|---------|---------|--------|------|-------|
| 0x00    | R-TYPE    | 1      | 0      | 0        | 1        | 0       | 0       | 0      | 0    | 10    |
| 0x20    | ADD       | 1      | 0      | 0        | 1        | 0       | 0       | 0      | 0    | 10    |
| 0x22    | SUB       | 1      | 0      | 0        | 1        | 0       | 0       | 0      | 0    | 10    |
| 0x18    | MULT      | 1      | 0      | 0        | 0        | 0       | 0       | 0      | 0    | 10    |
| 0x1A    | DIV       | 1      | 0      | 0        | 0        | 0       | 0       | 0      | 0    | 10    |
| 0x10    | MFHI      | 1      | 0      | 0        | 1        | 0       | 0       | 0      | 0    | 10    |
| 0x12    | MFLO      | 1      | 0      | 0        | 1        | 0       | 0       | 0      | 0    | 10    |
| 0x24    | AND       | 1      | 0      | 0        | 1        | 0       | 0       | 0      | 0    | 10    |
| 0x25    | OR        | 1      | 0      | 0        | 1        | 0       | 0       | 0      | 0    | 10    |
| 0x26    | XOR       | 1      | 0      | 0        | 1        | 0       | 0       | 0      | 0    | 10    |
| 0x27    | NOR       | 1      | 0      | 0        | 1        | 0       | 0       | 0      | 0    | 10    |
| 0x00    | SLL       | 1      | 1      | 0        | 1        | 0       | 0       | 0      | 0    | 10    |
| 0x02    | SRL       | 1      | 1      | 0        | 1        | 0       | 0       | 0      | 0    | 10    |
| 0x09    | JALR      | 1      | 0      | 0        | 1        | 0       | 0       | 0      | 1    | 10    |
| 0x08    | ADDI      | 0      | 1      | 0        | 1        | 0       | 0       | 0      | 0    | 00    |
| 0x0C    | ANDI      | 0      | 1      | 0        | 1        | 0       | 0       | 0      | 0    | 11    |
| 0x0D    | ORI       | 0      | 1      | 0        | 1        | 0       | 0       | 0      | 0    | 11    |
| 0x0E    | XORI      | 0      | 1      | 0        | 1        | 0       | 0       | 0      | 0    | 11    |
| 0x23    | LW        | 0      | 1      | 1        | 1        | 1       | 0       | 0      | 0    | 00    |
| 0x2B    | SW        | X      | 1      | X        | 0        | 0       | 1       | 0      | 0    | 00    |
| 0x11    | LB        | 0      | 1      | 1        | 1        | 1       | 0       | 0      | 0    | 00    |
| 0x21    | LH        | 0      | 1      | 1        | 1        | 1       | 0       | 0      | 0    | 00    |
| 0x28    | SB        | X      | 1      | X        | 0        | 0       | 1       | 0      | 0    | 00    |
| 0x29    | SH        | X      | 1      | X        | 0        | 0       | 1       | 0      | 0    | 00    |
| 0x04    | BEQ       | X      | 0      | X        | 0        | 0       | 0       | 1      | 0    | 01    |
| 0x05    | BNE       | X      | 0      | X        | 0        | 0       | 0       | 1      | 0    | 01    |
| 0x01    | BGEZ      | X      | 0      | X        | 0        | 0       | 0       | 1      | 0    | 01    |
| 0x07    | BGTZ      | X      | 0      | X        | 0        | 0       | 0       | 1      | 0    | 01    |
| 0x06    | BLEZ      | X      | 0      | X        | 0        | 0       | 0       | 1      | 0    | 01    |
| 0x3C    | BLTZ      | X      | 0      | X        | 0        | 0       | 0       | 1      | 0    | 01    |
| 0x0F    | LUI       | 0      | 1      | 0        | 1        | 0       | 0       | 0      | 0    | 11    |
| 0x39    | LI        | 0      | 1      | 0        | 1        | 0       | 0       | 0      | 0    | 11    |
| 0x33    | J         | X      | X      | X        | 0        | 0       | 0       | 0      | 1    | XX    |
| 0x03    | JAL       | X      | X      | X        | 1        | 0       | 0       | 0      | 1    | XX    |
| 0x0B    | JR        | X      | X      | X        | 0        | 0       | 0       | 0      | 1    | XX    |
| 0x3F    | SYSCALL   | X      | X      | X        | 0        | 0       | 0       | 0      | 0    | XX    |

- `X` indicates that the signal is not relevant for that instruction.
- `ALUOp` values: `00` = add/sub, `01` = branch comparison, `10` = R-type operation, `11` = bitwise ops.


# Control Signals in a 5-Bit Control Unit
## Control Signals Overview

### **ALUOp**
Defines the operation the ALU will perform.

| Value (Binary) | Effect |
|---------------|--------|
| `00` | The ALU performs an add operation. |
| `01` | The ALU performs a subtract operation. |
| `10` | The function field of the instruction determines the ALU operation. |

### **ALUSrcB**
Determines the second input to the ALU.

| Value (Binary) | Effect |
|---------------|--------|
| `00` | The second input to the ALU comes from the B register. |
| `01` | The second input to the ALU is the constant `4`. |
| `10` | The second input to the ALU is the sign-extended, lower 16 bits of the instruction register (IR). |
| `11` | The second input to the ALU is the sign-extended, lower 16 bits of the IR shifted left by 2 bits. |

### **PCSource**
Determines the source of the value written to the program counter (PC).

| Value (Binary) | Effect |
|---------------|--------|
| `00` | The output of the ALU (`PC + 4`) is sent to the PC for writing. |
| `01` | The contents of `ALUOut` (the branch target address) are sent to the PC for writing. |
| `10` | The jump target address (`IR[25:0]` shifted left by 2 bits and concatenated with `PC + 4[31:28]`) is sent to the PC for writing. |

## Notes
- `ALUOp` determines whether the ALU performs addition, subtraction, or an operation based on the instruction’s function field.
- `ALUSrcB` selects the second operand for ALU computations.
- `PCSource` selects the next address to be loaded into the PC.

