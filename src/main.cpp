#include <any>
#include <string>
#include <iostream>
#include <functional>
#include <unordered_map>

class EventEmitter
{
private:
    std::unordered_map<std::string, std::any> events;

public:
    EventEmitter() {}

    void on(const std::string &eventName, const std::any &eventCallback)
    {
        events[eventName] = eventCallback;
    }

    // Not needed since the function below now uses variadic templates
    // template <typename R>
    // R emit(const std::string &eventName)
    // {
    //     const std::any &eventCallback = events[eventName];
    //     return std::any_cast<std::function<R(void)>>(eventCallback)();
    // }

    template <typename R, typename... Args>
    R emit(const std::string &eventName, Args &&...args)
    {
        const std::any &eventCallback = events[eventName];
        return std::any_cast<std::function<R(Args...)>>(eventCallback)(std::forward<Args>(args)...);
    }

    virtual ~EventEmitter() {}
};

void fun1()
{
    std::cout << "fun1: no arguments" << std::endl;
}

double fun2(int i)
{
    double r = double(i);

    std::cout << "fun2: " << r << std::endl;

    return r;
}

double fun3(int x, int y)
{
    double r = double(x + y);

    std::cout << "fun3: " << r << std::endl;

    return r;
}

int main(int argc, char *argv[])
{
    EventEmitter e;

    e.on("fun1", std::function<void(void)>(fun1));
    e.on("fun2", std::function<double(int)>(fun2));

    e.emit<void>("fun1");
    e.emit<double, int>("fun2", 1);

    e.on("fun3", std::function<double(int, int)>(fun3));
    e.emit<double, int>("fun3", 1, 2);

    e.on("test", std::function<void(void)>([](){
        std::cout << "Hello, this is lambda test" << std::endl;
    }));

    e.emit<void>("test");

    return 0;
}