#ifndef BRAINFUCK_JIT_JIT_HPP
#define BRAINFUCK_JIT_JIT_HPP

#include "instruction.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stack>
#include <string>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

class JIT {

public:
    void print() {
        int x = 0;
        for (unsigned char i : code) {
            printf("\\x%x", i);
        }
        printf("\n");// flush!!
    }
    void compile() {
        unsigned int pos = 0;
        std::stack<int> loop_stack;
        //
        auto pushMachineCode = [this](const unsigned char inst[], int len) {
            // the `len - 1` means remove '\0' at the end of the c-style string
            for (int i = 0; i < len - 1; i++) {
                code.push_back(inst[i]);
            }
        };
        /*
        push rbp
        push r12
        push r13
        push r14
        mov rbp, [rsp - 30000]
         */
        const unsigned char start[] = "\x55\x41\x54\x41\x55\x41\x56\x48\x89\xe5\x48\x81\xed\x30\x75\x00\x00";
        /*
        pop r14
        pop r13
        pop r12
        pop rbp
        ret
        */
        const unsigned char end[] = "\x41\x5e\x41\x5d\x41\x5c\x5d\xc3";
        /*=------------------------------------------------------------------------=*/
        // add rbp, 0
        const unsigned char ptr_add[] = "\x48\x81\xc5\x00\x00\x00\x00";
        // sub rbp, 0
        const unsigned char ptr_sub[] = "\x48\x81\xed\x00\x00\x00\x00";
        // add byte ptr[rbp], 0
        const unsigned char val_add[] = "\x80\x45\x00\x00";
        // sub byte ptr[rbp], 0
        const unsigned char val_sub[] = "\x80\x6d\x00\x00";
        // putchar
        /*
        mov rdi, 1
        lea rsi, [rbp]
        mov rax, 1
        mov rdx, 1
        syscall
        */
        const unsigned char putchar[] = "\x48\xc7\xc7\x01\x00\x00\x00\x48\x8d\x75\x00\x48\xc7\xc0\x01\x00\x00\x00\x48\xc7\xc2\x01\x00\x00\x00\x0f\x05";
        // [
        // cmp byte ptr [rbp], 0
        // jne 9
        unsigned char lbracket[] = "\x80\x7d\x00\x00\x0f\x84\x00\x00\x00\x00";
        // ]
        // jmp ...
        unsigned char rbracket[] = "\xe9\x00\x00\x00\x00";

        //
        pushMachineCode(start, sizeof start);
        //

        for (auto &inst : instructions) {
            switch (inst->tag) {
                case Opcode::PTRADD:
                    pushMachineCode(ptr_add, sizeof ptr_add);
                    writeInt(code.size() - 4, static_cast<PtrAdd *>(inst)->times);
                    break;
                case Opcode::PTRSUB:
                    pushMachineCode(ptr_sub, sizeof ptr_sub);
                    writeInt(code.size() - 4, -static_cast<PtrSub *>(inst)->times);
                    break;
                case Opcode::VALADD:
                    pushMachineCode(val_add, sizeof val_add);
                    code.back() = static_cast<ValAdd *>(inst)->times & 0xff;
                    break;
                case Opcode::VALSUB:
                    pushMachineCode(val_sub, sizeof val_sub);
                    code.back() = -static_cast<ValSub *>(inst)->times & 0xff;
                    break;
                case Opcode::READCHAR:
                    break;
                case Opcode::PUTCHAR:
                    pushMachineCode(putchar, sizeof putchar);
                    break;
                case Opcode::LBRACKET:
                    loop_stack.push(code.size());
                    pushMachineCode(lbracket, sizeof lbracket);
                    loop_stack.push(code.size());
                    break;
                case Opcode::RBRACKET: {
                    pushMachineCode(rbracket, sizeof rbracket);
                    // relocation
                    auto idx = loop_stack.top();
                    loop_stack.pop();
                    auto idx2 = loop_stack.top();
                    loop_stack.pop();
                    writeInt(idx - 4, code.size() - idx);
                    writeInt(code.size() - 4, idx2 - code.size());
                    break;
                }
                case Opcode::UNKNOWN:
                    break;
            }
        }

        //
        pushMachineCode(end, sizeof end);
        //
    }

    void *allocMem(int size) {
        return mmap(nullptr, size, PROT_READ | PROT_WRITE,
                    MAP_ANON | MAP_PRIVATE, -1, 0);
    }
    void executeMem(void *mem) {
        memcpy(mem, code.data(), code.size());
        mprotect(mem, code.size(), PROT_READ | PROT_EXEC);
        auto func = reinterpret_cast<int (*)()>(mem);
        func();
    }

private:
    void writeInt(int start, int target) {
        // little end
        code[start + 3] = (target >> 24) & 0xff;
        code[start + 2] = (target >> 16) & 0xff;
        code[start + 1] = (target >> 8) & 0xff;
        code[start] = target & 0xff;
    }

private:
    std::vector<unsigned char> code;// mem

public:
    std::vector<Instruction *> instructions;
};


#endif//BRAINFUCK_JIT_JIT_HPP
