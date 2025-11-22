#include "ICalcFunction.h"
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
        return std::pow(left, right);
    }

    char getSymbol() override {
        return '^';
    }

    int getPriority() override {
        return 3;
    }
};

// C-совместимая функция экспорта
extern "C" __declspec(dllexport) IBinaryOperator* createBinaryOperator() {
    return new PowerOperator();
}