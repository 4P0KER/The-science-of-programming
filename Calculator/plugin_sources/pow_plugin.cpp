#include "IOperation.h"
#include <cmath>
#include <stdexcept>

class PowerOperator : public IBinaryOperator {
public:
    double execute(double left, double right) override {
        if (left == 0.0 && right < 0) {
            throw std::runtime_error("Zero to negative power is undefined");
        }
        if (left < 0 && std::fmod(right, 1.0) != 0.0) {
            throw std::runtime_error("Negative base with fractional exponent");
        }
        return pow(left, right);
    }

    char getSymbol() const override {
        return '^';
    }

    int getPriority() const override {
        return 3;
    }

    std::string getName() const override {
        return "^";
    }
};

extern "C" __declspec(dllexport) IOperation* createOperation() {
    return new PowerOperator();
}