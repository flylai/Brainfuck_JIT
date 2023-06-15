#ifndef BRAINFUCK_JIT_JIT_H
#define BRAINFUCK_JIT_JIT_H

#include "instruction.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <stack>
#include <string>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

class JitBase {

public:
    virtual void print() = 0;
    virtual void compile() = 0;

    virtual void *allocMem(int size) = 0;
    virtual void executeMem(void *mem) = 0;


public:
    std::vector<std::unique_ptr<Instruction>> instructions;
};


#endif//BRAINFUCK_JIT_JIT_H
