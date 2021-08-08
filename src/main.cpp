#include "instruction.hpp"
#include "jit.hpp"
#include "optimizer.hpp"
#include "vm.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <utility>
#include <vector>

//
bool OPTION_JIT = false;
bool OPTION_OPT = false;
std::vector<std::unique_ptr<Instruction>> instructions;
//
void showHelp() {
    std::string str = "Usage:\n";
    str += "    brainfuck_jit [options] <src file>\n";
    str += "where options include:\n";
    str += "    -jit enable jit(interpreter is default)\n";
    str += "    -opt enable optimizer\n";
    str += "    -h show this message\n";
    std::cout << str;
}

void compile(const std::string &code) {
    std::stack<unsigned int> loop_stack;

    for (auto c : code) {
        switch (c) {
            case '>':
                instructions.emplace_back(std::make_unique<PtrAdd>(1));
                break;
            case '<':
                instructions.emplace_back(std::make_unique<PtrSub>(-1));
                break;
            case '+':
                instructions.emplace_back(std::make_unique<ValAdd>(1));
                break;
            case '-':
                instructions.emplace_back(std::make_unique<ValSub>(-1));
                break;
            case '.':
                instructions.emplace_back(std::make_unique<PutChar>());
                break;
            case ',':
                instructions.emplace_back(std::make_unique<ReadChar>());
                break;
            case '[':
                loop_stack.push(instructions.size());
                instructions.emplace_back(std::make_unique<LBracket>(-1));
                break;
            case ']':
                instructions.emplace_back(std::make_unique<RBracket>(loop_stack.top()));
                static_cast<LBracket *>(instructions[loop_stack.top()].get())->target = instructions.size();
                loop_stack.pop();
                break;
            default:
                break;
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        showHelp();
        exit(-1);
    }

    for (int i = 1; i < argc; i++) {
        std::string option = std::string(argv[i]);
        if (option == "-jit") OPTION_JIT = true;
        else if (option == "-opt")
            OPTION_OPT = true;
        else if (option == "-h") {
            showHelp();
            exit(0);
        } else if (i != argc - 1) {
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
