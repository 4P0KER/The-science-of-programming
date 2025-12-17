#ifndef TEST_SUBJECT_H
#define TEST_SUBJECT_H

#include <string>
#include <iostream>

class TestSubject {
private:
    int value_ = 0;
    
public:
    TestSubject() = default;
    explicit TestSubject(int value) : value_(value) {}
    
    int multiply(int a, int b) {
        return a * b;
    }
    
    double divide(double a, double b) {
        if (b == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        return a / b;
    }
    
    std::string concatenate(const std::string& a, const std::string& b) {
        return a + b;
    }
    
    void print(const std::string& message) {
        std::cout << "Print: " << message << std::endl;
    }
    
    int no_params() {
        return 42;
    }
    
    int three_params(int x, int y, int z) {
        return x + y + z;
    }

    int get_value() const {
        return value_;
    }
    
    void set_value(int value) {
        value_ = value;
    }
};


class AnotherSubject {
public:
    int add(int a, int b) {
        return a + b;
    }
    
    std::string repeat(const std::string& s, int times) {
        std::string result;
        for (int i = 0; i < times; ++i) {
            result += s;
        }
        return result;
    }
};

#endif 