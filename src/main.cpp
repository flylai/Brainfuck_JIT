#include "instruction.hpp"
#include "jit.hpp"
#include "optimizer.hpp"
#include "vm.hpp"

#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

//
bool OPTION_JIT = false;
bool OPTION_OPT = false;
std::vector<Instruction *> instructions;
//
void showHelp() {
    std::string str = "Usage:\n";
    str += "    branfuck_jit [options] [src file]\n";
    str += "where options include:\n";
    str += "    -jit enable jit(interpreter is default)\n";
    str += "    -opt enable optimizer";
    std::cout << str;
}

void compile(const std::string &code) {
    std::stack<unsigned int> loop_stack;

    for (auto c : code) {
        switch (c) {
            case '>':
                instructions.emplace_back(new PtrAdd(1));
                break;
            case '<':
                instructions.emplace_back(new PtrSub(-1));
                break;
            case '+':
                instructions.emplace_back(new ValAdd(1));
                break;
            case '-':
                instructions.emplace_back(new ValSub(-1));
                break;
            case '.':
                instructions.emplace_back(new PutChar());
                break;
            case ',':
                instructions.emplace_back(new ReadChar());
                break;
            case '[':
                loop_stack.push(instructions.size());
                instructions.emplace_back(new LBracket(-1));
                break;
            case ']':
                instructions.emplace_back(new RBracket(loop_stack.top()));
                static_cast<LBracket *>(instructions[loop_stack.top()])->target = instructions.size();
                loop_stack.pop();
                break;
            default:
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc - 1; i++) {
        std::string option = std::string(argv[i]);
        if (option == "-jit") OPTION_JIT = true;
        else if (option == "-opt")
            OPTION_OPT = true;
        else {
            std::cerr << "Unknown option `" << option << "`\n";
            showHelp();
            exit(-1);
        }
    }


    std::ifstream in(argv[argc - 1], std::ios::in);
    std::string code((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    compile(code);

    if (OPTION_OPT) {
        Optimizer optimizer;
        optimizer.instructions = std::move(instructions);
        optimizer.optimize();
        instructions = std::move(optimizer.optimized);
    }

    if (OPTION_JIT) {
        JIT jit;
        jit.instructions = std::move(instructions);
        jit.compile();
        jit.executeMem(jit.allocMem(100000));
    } else {
        BrainfuckVM vm;
        vm.instructions = std::move(instructions);
        vm.run();
    }

    return 0;
}
