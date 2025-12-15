#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "IOperation.h"

class Calculator {
private:
    std::map<std::string, UnaryFunctionPtr> functions;
    std::map<char, BinaryOperatorPtr> binaryOperators;

    bool isOperator(char c) const;
    int getPriority(char op) const;
    std::string extractWord(const std::string& str, size_t& pos) const;
    double parseNumber(const std::string& str, size_t& pos) const;
    double evaluateExpression(const std::string& expr);
    double applyFunction(const std::string& funcName, double arg);
    double calculate(double a, double b, char op);

public:
    void registerFunction(const std::string& name, UnaryFunctionPtr func);
    void registerBinaryOperator(char symbol, BinaryOperatorPtr op);
    bool unregisterFunction(const std::string& name);
    bool unregisterBinaryOperator(char symbol);
    double evaluate(const std::string& expr);
    bool hasFunction(const std::string& name) const;
    bool hasBinaryOperator(char symbol) const;
    void listFunctions() const;
    void listBinaryOperators() const;
};