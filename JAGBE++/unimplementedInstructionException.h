#pragma once
#include <stdexcept>
#include <string>
class unimplementedInstructionException : public std::logic_error {
public:
    unimplementedInstructionException(uint8_t opcode);
    ~unimplementedInstructionException();
};
