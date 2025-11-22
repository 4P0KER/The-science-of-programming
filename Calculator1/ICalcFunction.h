#pragma once
#include <string>
#include <memory>

class ICalcFunction {
public:
    virtual ~ICalcFunction() = default;
    virtual double execute(double arg) = 0;
    virtual std::string getName() = 0;
};

class IBinaryOperator {
public:
    virtual ~IBinaryOperator() = default;
    virtual double execute(double left, double right) = 0;
    virtual char getSymbol() = 0;
    virtual int getPriority() = 0;
};

using CalcFunctionPtr = std::shared_ptr<ICalcFunction>;
using BinaryOperatorPtr = std::shared_ptr<IBinaryOperator>;