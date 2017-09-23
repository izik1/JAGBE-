#include "unimplementedInstructionException.h"
#include <sstream>
#include <iomanip>

inline std::string toHexString(uint8_t opcode) {
    std::stringstream str = std::stringstream();
    str << "0x" << std::setfill('0') << std::setw(2) << std::hex << int(opcode);
    return str.str();
}

unimplementedInstructionException::unimplementedInstructionException(uint8_t opcode) :
    std::logic_error("unimplemented instruction: " + toHexString(opcode)) { }

unimplementedInstructionException::~unimplementedInstructionException()
{
}
