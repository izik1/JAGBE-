#include "NotImplementedException.h"

NotImplementedException::NotImplementedException() : std::logic_error("Function not yet implemented") { };

NotImplementedException::NotImplementedException(const char* message) : std::logic_error(message) { };

NotImplementedException::~NotImplementedException()
{
}
