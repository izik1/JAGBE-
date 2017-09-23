#pragma once
#include <stdexcept>
class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException();
    NotImplementedException(const char* message);
    ~NotImplementedException();
};
