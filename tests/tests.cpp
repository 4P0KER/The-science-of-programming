#include "tests.h"
#include "../engine/Engine.h" 
#include "test_subject.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../engine/Wrapper.h"

void test_basic_functionality() {
    std::cout << "Test 1: Basic functionality" << std::endl;
    
    TestSubject subj;
    auto wrapper = create_wrapper(&subj, &TestSubject::multiply,
                                 {{"arg1", 0}, {"arg2", 0}});
    
    Engine engine;
    engine.register_command("command1", std::move(wrapper));
    
    int result = engine.execute_as<int>("command1", {{"arg1", 4}, {"arg2", 5}});
    assert(result == 20);
    std::cout << "  command1(4, 5) = " << result << std::endl;
}

void test_default_values() {
    std::cout << "\nTest 2: Default values" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    engine.register_command("multiply", 
        create_wrapper(&subj, &TestSubject::multiply,
                      {{"a", 10}, {"b", 20}}));
    
    int result1 = engine.execute_as<int>("multiply", {{"a", 3}, {"b", 7}});
    assert(result1 == 21);
    std::cout << "  multiply(3, 7) = " << result1 << std::endl;
    
    int result2 = engine.execute_as<int>("multiply", {{"a", 5}});
    assert(result2 == 100);
    std::cout << "  multiply(5) = " << result2 << std::endl;
    
    int result3 = engine.execute_as<int>("multiply", {});
    assert(result3 == 200);
    std::cout << "  multiply() = " << result3 << std::endl;
}

void test_different_return_types() {
    std::cout << "\nTest 3: Different return types" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    engine.register_command("divide", 
        create_wrapper(&subj, &TestSubject::divide,
                      {{"a", 1.0}, {"b", 1.0}}));
    
    engine.register_command("concat",
        create_wrapper(&subj, &TestSubject::concatenate,
                      {{"first", std::string("")}, {"second", std::string("")}}));
    
    double div_result = engine.execute_as<double>("divide", {{"a", 10.0}, {"b", 2.0}});
    assert(div_result == 5.0);
    std::cout << "  divide(10.0, 2.0) = " << div_result << std::endl;
    
    std::string concat_result = engine.execute_as<std::string>("concat",
        {{"first", std::string("Hello")}, {"second", std::string("World")}});
    assert(concat_result == "HelloWorld");
    std::cout << "  concat('Hello', 'World') = " << concat_result << std::endl;
}

void test_no_parameters() {
    std::cout << "\nTest 4: No parameters method" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    engine.register_command("answer", 
        create_wrapper(&subj, &TestSubject::no_params));
    
    int result = engine.execute_as<int>("answer", {});
    assert(result == 42);
    std::cout << "  answer() = " << result << std::endl;
}

void test_void_method() {
    std::cout << "\nTest 5: Void method" << std::endl;
    
    TestSubject subj;
    Engine engine;

    std::stringstream buffer;
    auto old_cout = std::cout.rdbuf(buffer.rdbuf());
    
    engine.register_command("print", 
        create_wrapper(&subj, &TestSubject::print,
                      {{"message", std::string("")}}));
    
    engine.execute("print", {{"message", std::string("Test")}});
    
    std::cout.rdbuf(old_cout);
    std::string output = buffer.str();
    assert(output.find("Test") != std::string::npos);
    std::cout << " Print('Test') executed" << std::endl;
}

void test_error_handling() {
    std::cout << "\nTest 6: Error handling" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    engine.register_command("multiply",
        create_wrapper(&subj, &TestSubject::multiply,
                      {{"a", 0}, {"b", 0}}));

    bool exception_caught = false;
    try {
        engine.execute("nonexistent", {});
    } catch (const std::runtime_error&) {
        exception_caught = true;
    }
    assert(exception_caught);
    std::cout << "  Nonexistent command throws" << std::endl;

    exception_caught = false;
    try {
        engine.execute("multiply", {{"a", std::string("wrong")}, {"b", 5}});
    } catch (const std::runtime_error&) {
        exception_caught = true;
    }
    assert(exception_caught);
    std::cout << "  Wrong argument type throws" << std::endl;
}

void test_command_management() {
    std::cout << "\nTest 7: Command management" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    assert(engine.command_count() == 0);
    assert(engine.get_command_list().empty());
    
    engine.register_command("cmd1",
        create_wrapper(&subj, &TestSubject::multiply,
                      {{"a", 0}, {"b", 0}}));
    
    assert(engine.command_count() == 1);
    assert(engine.has_command("cmd1"));
    std::cout << "  ✓ Command registered" << std::endl;
    
    engine.remove_command("cmd1");
    assert(engine.command_count() == 0);
    assert(!engine.has_command("cmd1"));
    std::cout << "  Command removed" << std::endl;
    
    engine.register_command("cmd1",
        create_wrapper(&subj, &TestSubject::multiply,
                      {{"a", 0}, {"b", 0}}));
    engine.register_command("cmd2",
        create_wrapper(&subj, &TestSubject::divide,
                      {{"a", 1.0}, {"b", 1.0}}));
    
    assert(engine.command_count() == 2);
    
    engine.clear();
    assert(engine.command_count() == 0);
    std::cout << "  Engine cleared" << std::endl;
}

void test_const_methods() {
    std::cout << "\nTest 8: Const methods" << std::endl;
    
    TestSubject subj(100);
    Engine engine;
    
    engine.register_command("get_value",
        create_wrapper(&subj, &TestSubject::get_value));
    
    int result = engine.execute_as<int>("get_value", {});
    assert(result == 100);
    std::cout << "  get_value() = " << result << std::endl;
}

void test_multiple_objects() {
    std::cout << "\nTest 9: Multiple objects" << std::endl;
    
    TestSubject subj1;
    AnotherSubject subj2;
    Engine engine;
    
    engine.register_command("multiply",
        create_wrapper(&subj1, &TestSubject::multiply,
                      {{"a", 0}, {"b", 0}}));
    
    engine.register_command("add",
        create_wrapper(&subj2, &AnotherSubject::add,
                      {{"x", 0}, {"y", 0}}));
    
    int result1 = engine.execute_as<int>("multiply", {{"a", 3}, {"b", 4}});
    int result2 = engine.execute_as<int>("add", {{"x", 3}, {"y", 4}});
    
    assert(result1 == 12);
    assert(result2 == 7);
    std::cout << "  multiply(3,4) = " << result1 << std::endl;
    std::cout << "  add(3,4) = " << result2 << std::endl;
}

void test_get_parameters() {
    std::cout << "\nTest 10: Get command parameters" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    engine.register_command("three_params",
        create_wrapper(&subj, &TestSubject::three_params));
    
    auto params = engine.get_command_params("three_params");
    assert(params.size() == 3);
    assert(params[0] == "arg1");
    assert(params[1] == "arg2");
    assert(params[2] == "arg3");
    std::cout << "  Parameters: " << params[0] << ", " << params[1] << ", " << params[2] << std::endl;
}

void test_argument_order() {
    std::cout << "\nTest 11: Argument order" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    engine.register_command("multiply",
        create_wrapper(&subj, &TestSubject::multiply,
                      {{"first", 0}, {"second", 0}}));
    
    int result1 = engine.execute_as<int>("multiply", {{"first", 3}, {"second", 4}});
    int result2 = engine.execute_as<int>("multiply", {{"second", 4}, {"first", 3}});
    
    assert(result1 == 12);
    assert(result2 == 12);
    std::cout << "  Order doesn't matter: " << result1 << " = " << result2 << std::endl;
}

void test_duplicate_arguments() {
    std::cout << "\nTest 12: Duplicate arguments" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    engine.register_command("multiply",
        create_wrapper(&subj, &TestSubject::multiply,
                      {{"a", 0}, {"b", 0}}));
    
    bool exception_caught = false;
    try {
        engine.execute("multiply", {{"a", 1}, {"a", 2}, {"b", 3}});
    } catch (const std::runtime_error&) {
        exception_caught = true;
    }
    assert(exception_caught);
    std::cout << "  Duplicate arguments throw" << std::endl;
}

void test_method_exceptions() {
    std::cout << "\nTest 13: Method exceptions" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    engine.register_command("divide",
        create_wrapper(&subj, &TestSubject::divide,
                      {{"a", 1.0}, {"b", 1.0}}));
    
    bool exception_caught = false;
    try {
        engine.execute("divide", {{"a", 10.0}, {"b", 0.0}});
    } catch (const std::runtime_error& e) {
        exception_caught = true;
        assert(std::string(e.what()).find("Division by zero") != std::string::npos);
        std::cout << "  Division by zero throws: " << e.what() << std::endl;
    }
    assert(exception_caught);
}

void test_return_type_checking() {
    std::cout << "\nTest 14: Return type checking" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    engine.register_command("get_string",
        create_wrapper(&subj, &TestSubject::concatenate,
                      {{"first", std::string("")}, {"second", std::string("")}}));
    
    std::string result = engine.execute_as<std::string>("get_string",
        {{"first", std::string("Hello")}, {"second", std::string("World")}});
    assert(result == "HelloWorld");
    std::cout << "  Correct return type works" << std::endl;
    
    bool exception_caught = false;
    try {
        int wrong_result = engine.execute_as<int>("get_string",
            {{"first", std::string("Hello")}, {"second", std::string("World")}});
        (void)wrong_result;
    } catch (const std::runtime_error&) {
        exception_caught = true;
        std::cout << " Wrong return type throws" << std::endl;
    }
    assert(exception_caught);
}

void test_argument_reordering() {
    std::cout << "\nTest 15: Argument reordering" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    engine.register_command("complex",
        create_wrapper(&subj, &TestSubject::three_params));

    int result = engine.execute_as<int>("complex",
        {{"arg3", 4}, {"arg1", 1}, {"arg2", 2}});
    assert(result == 7); 
    std::cout << " Arguments in reverse order work: " << result << std::endl;

    result = engine.execute_as<int>("complex",
        {{"arg2", 10}, {"arg3", 20}, {"arg1", 30}});
    assert(result == 60); 
    std::cout << " Arguments in random order work: " << result << std::endl;
}

void test_engine_overflow() {
    std::cout << "\nTest 16: Engine capacity" << std::endl;
    
    TestSubject subj;
    Engine engine;

    const int NUM_COMMANDS = 100;
    for (int i = 0; i < NUM_COMMANDS; ++i) {
        std::string name = "cmd" + std::to_string(i);
        engine.register_command(name,
            create_wrapper(&subj, &TestSubject::multiply,
                          {{"a", i}, {"b", i+1}}));
    }
    
    assert(engine.command_count() == NUM_COMMANDS);
    std::cout << "  Registered " << NUM_COMMANDS << " commands" << std::endl;

    for (int i = 0; i < NUM_COMMANDS; ++i) {
        std::string name = "cmd" + std::to_string(i);
        int result = engine.execute_as<int>(name, {});
        assert(result == i * (i + 1));
    }
    std::cout << " All commands work correctly" << std::endl;
}

void test_various_default_types() {
    std::cout << "\nTest 17: Various default value types" << std::endl;
    
    TestSubject subj;
    Engine engine;

    engine.register_command("bool_test",
        create_wrapper(&subj, &TestSubject::concatenate,
                      {{"first", std::string("true:")}, 
                       {"second", std::string("false")}}));
    
    engine.register_command("int_test",
        create_wrapper(&subj, &TestSubject::multiply,
                      {{"a", 7}, {"b", 8}}));
    
    engine.register_command("double_test",
        create_wrapper(&subj, &TestSubject::divide,
                      {{"a", 100.0}, {"b", 4.0}}));

    std::string str_result = engine.execute_as<std::string>("bool_test", {});
    assert(str_result == "true:false");
    std::cout << " String defaults work: \"" << str_result << "\"" << std::endl;
    
    int int_result = engine.execute_as<int>("int_test", {});
    assert(int_result == 56);
    std::cout << " Int defaults work: " << int_result << std::endl;
    
    double double_result = engine.execute_as<double>("double_test", {});
    assert(double_result == 25.0);
    std::cout << " Double defaults work: " << double_result << std::endl;
}

void test_empty_command_registration() {
    std::cout << "\nTest 18: Empty command registration" << std::endl;
    
    Engine engine;
    
    bool exception_caught = false;
    try {
        engine.register_command("", 
            create_wrapper(&TestSubject(), &TestSubject::no_params));
    } catch (const std::runtime_error&) {
        exception_caught = true;
        std::cout << " Empty command name throws" << std::endl;
    }
    assert(exception_caught);
}

void test_null_pointer_safety() {
    std::cout << "\nTest 19: Null pointer safety" << std::endl;
    
    bool exception_caught = false;
    try {
        TestSubject* null_subj = nullptr;
        auto wrapper = create_wrapper(null_subj, &TestSubject::no_params);
    } catch (const std::runtime_error& e) {
        exception_caught = true;
        assert(std::string(e.what()).find("null") != std::string::npos);
        std::cout << "  ✓ Null pointer throws: " << e.what() << std::endl;
    }
    assert(exception_caught);
}

void test_engine_reuse() {
    std::cout << "\nTest 20: Engine reuse after clear" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    engine.register_command("cmd1",
        create_wrapper(&subj, &TestSubject::multiply,
                      {{"a", 0}, {"b", 0}}));
    engine.register_command("cmd2",
        create_wrapper(&subj, &TestSubject::divide,
                      {{"a", 1.0}, {"b", 1.0}}));
    
    assert(engine.command_count() == 2);
    
    engine.clear();
    assert(engine.command_count() == 0);
    
    engine.register_command("new_cmd",
        create_wrapper(&subj, &TestSubject::no_params));
    
    assert(engine.command_count() == 1);
    int result = engine.execute_as<int>("new_cmd", {});
    assert(result == 42);
    
    std::cout << "Engine can be reused after clear" << std::endl;
}

void test_default_type_mismatch() {
    std::cout << "\nTest 21: Default value type mismatch" << std::endl;
    
    TestSubject subj;
    
    bool exception_caught = false;
    try {
        auto wrapper = create_wrapper(&subj, &TestSubject::multiply,
                                     {{"a", std::string("wrong")}, {"b", 0}});
    } catch (const std::runtime_error& e) {
        exception_caught = true;
        assert(std::string(e.what()).find("type mismatch") != std::string::npos ||
               std::string(e.what()).find("Default value") != std::string::npos);
        std::cout << " Default type mismatch throws: " << e.what() << std::endl;
    }
    assert(exception_caught);
}

void test_get_params_nonexistent() {
    std::cout << "\nTest 22: Get parameters of nonexistent command" << std::endl;
    
    Engine engine;
    
    bool exception_caught = false;
    try {
        auto params = engine.get_command_params("ghost");
        (void)params; // подавляем warning
    } catch (const std::runtime_error& e) {
        exception_caught = true;
        assert(std::string(e.what()).find("not found") != std::string::npos);
        std::cout << "Get params of nonexistent command throws: " << e.what() << std::endl;
    }
    assert(exception_caught);
}

void test_void_return_handling() {
    std::cout << "\nTest 23: Void return handling" << std::endl;
    
    TestSubject subj;
    Engine engine;
    
    engine.register_command("print_void",
        create_wrapper(&subj, &TestSubject::print,
                      {{"message", std::string("default")}}));
    
    // Перехватываем вывод
    std::stringstream buffer;
    auto old_cout = std::cout.rdbuf(buffer.rdbuf());
    
    // Выполняем void метод
    std::any result = engine.execute("print_void", {{"message", std::string("void_test")}});
    
    std::cout.rdbuf(old_cout);
    
    // Проверяем, что метод выполнился
    std::string output = buffer.str();
    assert(output.find("void_test") != std::string::npos);
    
    // Проверяем, что возвращается пустой any
    assert(result.type() == typeid(void));
    
    std::cout << "  Void method executes and returns empty any" << std::endl;
}

void test_another_subject_comprehensive() {
    std::cout << "\nTest 24: AnotherSubject comprehensive" << std::endl;
    
    AnotherSubject subj;
    Engine engine;
    
    engine.register_command("repeat",
        create_wrapper(&subj, &AnotherSubject::repeat,
                      {{"s", std::string("")}, {"times", 1}}));
    
    std::string result1 = engine.execute_as<std::string>("repeat", {});
    assert(result1 == "");
    std::cout << "  Default values work for AnotherSubject" << std::endl;
    
    std::string result2 = engine.execute_as<std::string>("repeat",
        {{"s", std::string("AB")}, {"times", 3}});
    assert(result2 == "ABABAB");
    std::cout << "  Method with two parameters works: \"" << result2 << "\"" << std::endl;
}

void test_many_arguments() {
    std::cout << "\nTest 25: Stress test with many arguments" << std::endl;
    
    class ManyArgsSubject {
    public:
        int sum10(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
            return a + b + c + d + e + f + g + h + i + j;
        }
    };
    
    ManyArgsSubject subj;
    Engine engine;
    
    auto wrapper = create_wrapper(&subj, &ManyArgsSubject::sum10);
    
    engine.register_command("sum10", std::move(wrapper));
    
    int result = engine.execute_as<int>("sum10", {
        {"arg1", 1}, {"arg2", 2}, {"arg3", 3}, {"arg4", 4}, {"arg5", 5},
        {"arg6", 6}, {"arg7", 7}, {"arg8", 8}, {"arg9", 9}, {"arg10", 10}
    });
    
    assert(result == 55); 
    std::cout << "Works: " << result << std::endl;
}

void run_all_tests() {
  
    try {
        test_basic_functionality();
        test_default_values();
        test_different_return_types();
        test_no_parameters();
        test_void_method();
        test_error_handling();
        test_command_management();
        test_const_methods();
        test_multiple_objects();
        test_get_parameters();
        test_argument_order();
        test_duplicate_arguments();
        test_method_exceptions();
        test_return_type_checking();
        test_argument_reordering();
        test_engine_overflow();
        test_various_default_types();
        test_empty_command_registration();
        test_null_pointer_safety();
        test_engine_reuse();
        test_default_type_mismatch();
        test_get_params_nonexistent();
        test_void_return_handling();
        test_another_subject_comprehensive();
        test_many_arguments();
        
        std::cout << "tests passed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nTest failed: " << e.what() << std::endl;
        throw;
    } catch (...) {
        std::cerr << "\nUnknown test failure" << std::endl;
        throw;
    }
}