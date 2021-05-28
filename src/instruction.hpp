#ifndef BRAINFUCK_JIT_INSTRUCTION_H
#define BRAINFUCK_JIT_INSTRUCTION_H


enum class Opcode : char {
    PTRADD,  // >
    PTRSUB,  // <
    VALADD,  // +
    VALSUB,  // -
    READCHAR,// ,
    PUTCHAR, // .
    LBRACKET,// [
    RBRACKET,//]
    UNKNOWN,
};

struct Instruction {
    Opcode tag{Opcode::UNKNOWN};
};

struct PtrValCompute : Instruction {
    unsigned int times = 0;
};

struct PtrAdd : PtrValCompute {
    explicit PtrAdd(unsigned int times) {
        this->times = times;
        tag = Opcode::PTRADD;
    }
};
struct PtrSub : PtrValCompute {
    explicit PtrSub(unsigned int times) {
        this->times = times;
        tag = Opcode::PTRSUB;
    }
};
struct ValAdd : PtrValCompute {
    explicit ValAdd(unsigned int times) {
        this->times = times;
        tag = Opcode::VALADD;
    }
};
struct ValSub : PtrValCompute {
    explicit ValSub(unsigned int times) {
        this->times = times;
        tag = Opcode::VALSUB;
    }
};
struct ReadChar : Instruction {
    ReadChar() { tag = Opcode::READCHAR; }
};
struct PutChar : Instruction {
    PutChar() { tag = Opcode::PUTCHAR; }
};

struct Jmp : Instruction {
    unsigned int target = 0;
};
struct LBracket : Jmp {
    explicit LBracket(unsigned int target) {
        this->target = target;
        tag = Opcode::LBRACKET;
    }
};
struct RBracket : Jmp {
    explicit RBracket(unsigned int target) {
        this->target = target;
        tag = Opcode::RBRACKET;
    }
};

#endif//BRAINFUCK_JIT_INSTRUCTION_H
