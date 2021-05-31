#include "instruction.hpp"
#include "optimizer.hpp"
#include "vm.hpp"

#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

int main(int argc, char *argv[]) {
    std::ifstream in(argv[1], std::ios::in);
    std::string code((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    std::vector<Instruction *> instructions;
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

    BrainfuckVM vm;

    {
        Optimizer optimizer;
        optimizer.instructions = std::move(instructions);
        optimizer.optimize();
        vm.instructions = std::move(optimizer.optimized);
    }


    vm.run();

    return 0;
}
