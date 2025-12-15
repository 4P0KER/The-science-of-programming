#include "IOperation.h"
#include <cmath>

class SinFunction : public IUnaryFunction {
public:
    double execute(double arg) override {
        return sin(arg * 3.14159265358979323846 / 180.0);
    }

    std::string getName() const override {
        return "sin";
    }
};

extern "C" __declspec(dllexport) IOperation* createOperation() {
    return new SinFunction();
}