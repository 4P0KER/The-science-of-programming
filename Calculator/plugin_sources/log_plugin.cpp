#include "IOperation.h"
#include <cmath>
#include <stdexcept>

class Log10Function : public IUnaryFunction {
public:
    double execute(double arg) override {
        if (arg <= 0.0) {
            throw std::runtime_error("Logarithm argument must be positive");
        }
        return log10(arg);
    }

    std::string getName() const override {
        return "log";
    }
};

extern "C" __declspec(dllexport) IOperation* createOperation() {
    return new Log10Function();
}