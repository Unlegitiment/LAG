#include "exception.h"
#include <string>
LagException::LagException(const std::string& exeString) : std::exception(exeString.c_str())
{
}
