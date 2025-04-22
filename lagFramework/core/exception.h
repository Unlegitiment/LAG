#pragma once
#include<exception>
#include <string>

class LagException : public std::exception {
public:
    LagException(const std::string& exeString);
};
class InvalidShaderAccess : public LagException {
public:
    InvalidShaderAccess(const std::string& line, const std::string& function) : LagException("[FATAL]: Undefined Shader Type. @" + line + "Func: " + function) { }
};