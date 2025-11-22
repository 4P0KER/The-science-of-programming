#include "ICalcFunction.h"
#include <cmath>
#include <string>
#define PI 3.14159265

class SinFunction : public ICalcFunction {
public:
    double execute(double arg) override {
        return sin(arg * PI / 180);
    }

    std::string getName() override {
        return "sin";
    }
};

// C-совместимая функция экспорта
extern "C" __declspec(dllexport) ICalcFunction* createFunction() {
    return new SinFunction();
}