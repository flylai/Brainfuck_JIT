#ifndef BRAINFUCK_ASMJIT_JIT_HPP
#define BRAINFUCK_ASMJIT_JIT_HPP

#include "asmjit/x86.h"
#include "jit.h"

using namespace asmjit;

class AsmJit : public JitBase {
public:
    void print() override {
    }
    void compile() override {
        code_holder.init(rt.environment(), rt.cpuFeatures());
        x86::Assembler assembler(&code_holder);

        start(assembler);
        for (auto &inst : instructions) {
            switch (inst->tag) {
                case Opcode::PTRADD:
                    ptrAdd(assembler, static_cast<PtrAdd *>(inst.get())->times);
                    break;
                case Opcode::PTRSUB:
                    ptrSub(assembler, -static_cast<PtrSub *>(inst.get())->times);
                    break;
                case Opcode::VALADD:
                    valAdd(assembler, static_cast<ValAdd *>(inst.get())->times & 0xff);
                    break;
                case Opcode::VALSUB:
                    valSub(assembler, -static_cast<ValSub *>(inst.get())->times & 0xff);
                    break;
                case Opcode::READCHAR:
                    readChar(assembler);
                    break;
                case Opcode::PUTCHAR:
                    putChar(assembler);
                    break;
                case Opcode::LBRACKET:
                    leftBracket(assembler);
                    break;
                case Opcode::RBRACKET: {
                    rightBracket(assembler);
                    break;
                }
                case Opcode::UNKNOWN:
                    break;
            }
        }
        end(assembler);
    }
    void *allocMem(int size) override {
        return nullptr;
    }
    void executeMem(void *mem) override {
        typedef void (*Func)();
        Func fn;
        Error err = rt.add(&fn, &code_holder);
        if (err) {
            printf("AsmJit failed: %s\n", DebugUtils::errorAsString(err));
        }
        fn();
        rt.release(fn);
    }


private:
    void start(x86::Assembler &assembler) {
        assembler.push(x86::rbp);
        assembler.push(x86::r12);
        assembler.push(x86::r13);
        assembler.push(x86::r14);
        assembler.mov(x86::rbp, x86::rsp);
        assembler.sub(x86::rbp, 0x7530);
    }

    void end(x86::Assembler &assembler) {
        assembler.pop(x86::r14);
        assembler.pop(x86::r13);
        assembler.pop(x86::r12);
        assembler.pop(x86::rbp);
        assembler.ret();
    }

    void ptrAdd(x86::Assembler &assembler, int times) {
        assembler.add(x86::rbp, times);
    }

    void ptrSub(x86::Assembler &assembler, int times) {
        assembler.sub(x86::rbp, times);
    }

    void valAdd(x86::Assembler &assembler, int times) {
        assembler.add(x86::byte_ptr(x86::rbp), times);
    }

    void valSub(x86::Assembler &assembler, int times) {
        assembler.sub(x86::byte_ptr(x86::rbp), times);
    }

    void putChar(x86::Assembler &assembler) {
        assembler.mov(x86::rdi, 1);
        assembler.lea(x86::rsi, x86::ptr(x86::rbp));
        assembler.mov(x86::rax, 1);
        assembler.mov(x86::rdx, 1);
        assembler.syscall();
    }

    void readChar(x86::Assembler &assembler) {
        assembler.mov(x86::rax, 0);
        assembler.mov(x86::rdi, 0);
        assembler.lea(x86::rsi, x86::ptr(x86::rbp));
        assembler.mov(x86::rdx, 1);
        assembler.syscall();
    }

    void leftBracket(x86::Assembler &assembler) {
        auto l1 = assembler.newLabel();
        auto l2 = assembler.newLabel();
        label.push(l1);
        label.push(l2);
        assembler.bind(l1);
        assembler.cmp(x86::byte_ptr(x86::rbp), 0);
        assembler.je(l2);
    }

    void rightBracket(x86::Assembler &assembler) {
        auto l2 = label.top();
        label.pop();
        auto l1 = label.top();
        label.pop();
        assembler.jmp(l1);
        assembler.bind(l2);
    }

private:
    std::stack<Label> label;
    JitRuntime rt;
    CodeHolder code_holder;
};

#endif// BRAINFUCK_ASMJIT_JIT_HPP