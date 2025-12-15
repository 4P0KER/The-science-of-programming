#include "IOperation.h"
#include <cmath>
#include <stdexcept>

class LnFunction : public IUnaryFunction {
public:
    double execute(double arg) override {
        if (arg <= 0.0) {
            throw std::runtime_error("Logarithm argument must be positive");
        }
        return log(arg);
    }

    std::string getName() const override {
        return "ln";
    }
};

extern "C" __declspec(dllexport) IOperation* createOperation() {
    return new LnFunction();
}