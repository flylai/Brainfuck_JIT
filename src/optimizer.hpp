#ifndef BRAINFUCK_JIT_OPTIMIZER_HPP
#define BRAINFUCK_JIT_OPTIMIZER_HPP

#include "instruction.hpp"

#include <iostream>
#include <memory>
#include <stack>
#include <vector>

template<typename T, typename... R>
bool inOr(T lhs, R... rhs) {
    return ((lhs == rhs) || ...);
}


class Optimizer {
public:
    void optimize() {
        for (auto &inst : instructions) {
            // backup inst->tag, for std::move(inst) when opcode is one of
            // Opcode::READCHAR, Opcode::PUTCHAR, Opcode::LBRACKET, Opcode::RBRACKET
            auto tmp = inst->tag;
            if (inOr(inst->tag, Opcode::PTRADD, Opcode::PTRSUB) && inOr(pre_inst, Opcode::PTRADD, Opcode::PTRSUB)) {
                if (inst->tag == Opcode::PTRADD) {
                    count++;
                } else {
                    count--;
                }
            } else if (inOr(inst->tag, Opcode::VALADD, Opcode::VALSUB) && inOr(pre_inst, Opcode::VALADD, Opcode::VALSUB)) {
                if (inst->tag == Opcode::VALADD) {
                    count++;
                } else {
                    count--;
                }
            } else {
                if (inOr(pre_inst, Opcode::PTRADD, Opcode::PTRSUB)) {
                    if (count > 0) {
                        optimized.push_back(std::make_unique<PtrAdd>(count));
                    } else if (count < 0) {
                        optimized.push_back(std::make_unique<PtrSub>(count));
                    }
                    count = 0;
                } else if (inOr(pre_inst, Opcode::VALADD, Opcode::VALSUB)) {
                    if (count > 0) {
                        optimized.push_back(std::make_unique<ValAdd>(count));
                    } else if (count < 0) {
                        optimized.push_back(std::make_unique<ValSub>(count));
                    }
                    count = 0;
                }
                if (inOr(inst->tag, Opcode::READCHAR, Opcode::PUTCHAR, Opcode::LBRACKET, Opcode::RBRACKET)) {
                    optimized.push_back(std::move(inst));
                    count = 0;
                } else if (inOr(inst->tag, Opcode::VALADD, Opcode::PTRADD)) {
                    count = 1;
                } else if (inOr(inst->tag, Opcode::VALSUB, Opcode::PTRSUB)) {
                    count = -1;
                }
            }
            pre_inst = tmp;
        }
        relocation();
    }
    void relocation() {
        std::stack<unsigned int> loop;
        for (unsigned int i = 0; i < optimized.size(); i++) {
            if (optimized[i]->tag == Opcode::LBRACKET) loop.push(i);
            else if (optimized[i]->tag == Opcode::RBRACKET) {
                auto tmp = loop.top();
                loop.pop();
                static_cast<LBracket *>(optimized[tmp].get())->target = i + 1;
                static_cast<RBracket *>(optimized[i].get())->target = tmp;
            }
        }
    }
    void debug() {
        int cnt = 0;
        for (const auto &inst : optimized) {
            std::cout << inst->toString() << ", ";
            if (++cnt == 8) {
                std::cout << std::endl;
                cnt = 0;
            }
        }
        std::cout << std::flush;
    }

public:
    std::vector<std::unique_ptr<Instruction>> instructions;
    std::vector<std::unique_ptr<Instruction>> optimized;

private:
    int count{0};
    Opcode pre_inst{Opcode::UNKNOWN};
};

#endif//BRAINFUCK_JIT_OPTIMIZER_HPP
