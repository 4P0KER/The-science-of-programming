#pragma once
#include <string>
#include <memory>

class IOperation {
public:
    virtual ~IOperation() = default;
    virtual std::string getName() const = 0;
};

class IUnaryFunction : public IOperation {
public:
    virtual double execute(double arg) = 0;
};

class IBinaryOperator : public IOperation {
public:
    virtual double execute(double left, double right) = 0;
    virtual char getSymbol() const = 0;
    virtual int getPriority() const = 0;
};

using UnaryFunctionPtr = std::shared_ptr<IUnaryFunction>;
using BinaryOperatorPtr = std::shared_ptr<IBinaryOperator>;