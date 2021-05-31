#ifndef BRAINFUCK_JIT_VM_H
#define BRAINFUCK_JIT_VM_H

#include "instruction.hpp"
#include <cstdio>
#include <vector>

class BrainfuckVM {
public:
    void run() {
        while (pc < instructions.size()) {
            switch (instructions[pc]->tag) {
                case Opcode::PTRADD:
                    ptrAdd();
                    break;
                case Opcode::PTRSUB:
                    ptrSub();
                    break;
                case Opcode::VALADD:
                    valAdd();
                    break;
                case Opcode::VALSUB:
                    valSub();
                    break;
                case Opcode::READCHAR:
                    readChar();
                    break;
                case Opcode::PUTCHAR:
                    putChar();
                    break;
                case Opcode::LBRACKET:
                    lBracket();
                    break;
                case Opcode::RBRACKET:
                    rBracket();
                    break;
                default:
                    break;
            }
            pc++;
        }
    }

private:
    void ptrAdd() { sp += static_cast<PtrAdd *>(instructions[pc])->times; }
    void ptrSub() { sp += static_cast<PtrSub *>(instructions[pc])->times; }
    void valAdd() { registers[sp] += static_cast<ValAdd *>(instructions[pc])->times; }
    void valSub() { registers[sp] += static_cast<ValSub *>(instructions[pc])->times; }
    void readChar() { registers[sp] = getchar(); }
    void putChar() { putchar(registers[sp]); }
    void lBracket() {
        if (registers[sp] == 0) {
            pc = static_cast<LBracket *>(instructions[pc])->target - 1;
        }
    }
    void rBracket() { pc = static_cast<RBracket *>(instructions[pc])->target - 1; }

private:
    unsigned int pc{0};
    unsigned int sp{0};

public:
    BrainfuckVM() { registers = std::vector<char>(1000, 0); };
    ~BrainfuckVM() {
        for (auto &inst : instructions) { delete inst; }
    }
    std::vector<Instruction *> instructions;
    std::vector<char> registers;
};

#endif//BRAINFUCK_JIT_VM_H
