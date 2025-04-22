#include "exception.h"
LagException::LagException(const std::string& exeString) : std::exception(exeString.c_str())
{
}
