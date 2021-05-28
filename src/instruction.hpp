#ifndef BRAINFUCK_JIT_INSTRUCTION_H
#define BRAINFUCK_JIT_INSTRUCTION_H

#include <string>

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
    virtual std::string toString() { return "Instruction(UNKNOWN)"; }
};

struct PtrValCompute : Instruction {
    int times = 0;
};

struct PtrAdd : PtrValCompute {
    explicit PtrAdd(int times) {
        this->times = times;
        tag = Opcode::PTRADD;
    }
    std::string toString() override {
        return "PtrAdd(" + std::to_string(times) + ")";
    }
};
struct PtrSub : PtrValCompute {
    explicit PtrSub(int times) {
        this->times = times;
        tag = Opcode::PTRSUB;
    }
    std::string toString() override {
        return "PtrSub(" + std::to_string(-times) + ")";
    }
};
struct ValAdd : PtrValCompute {
    explicit ValAdd(int times) {
        this->times = times;
        tag = Opcode::VALADD;
    }
    std::string toString() override {
        return "ValAdd(" + std::to_string(times) + ")";
    }
};
struct ValSub : PtrValCompute {
    explicit ValSub(int times) {
        this->times = times;
        tag = Opcode::VALSUB;
    }
    std::string toString() override {
        return "ValSub(" + std::to_string(-times) + ")";
    }
};
struct ReadChar : Instruction {
    ReadChar() { tag = Opcode::READCHAR; }
    std::string toString() override {
        return "ReadChar()";
    }
};
struct PutChar : Instruction {
    PutChar() { tag = Opcode::PUTCHAR; }
    std::string toString() override {
        return "PutChar()";
    }
};

struct Jmp : Instruction {
    unsigned int target = 0;
};
struct LBracket : Jmp {
    explicit LBracket(unsigned int target) {
        this->target = target;
        tag = Opcode::LBRACKET;
    }
    std::string toString() override {
        return "LBracket(" + std::to_string(target) + ")";
    }
};
struct RBracket : Jmp {
    explicit RBracket(unsigned int target) {
        this->target = target;
        tag = Opcode::RBRACKET;
    }
    std::string toString() override {
        return "RBracket(" + std::to_string(target) + ")";
    }
};

#endif//BRAINFUCK_JIT_INSTRUCTION_H
