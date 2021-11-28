#include <any>
#include <string>
#include <iostream>
#include <functional>
#include <unordered_map>

class Person
{
private:
    std::string name;
    int age;
public:
    Person(const std::string &p_name) : name(p_name) {}
    ~Person() {}
    void introduce()
    {
        std::cout << "Hi I'm a Person and my name is " << this->name << std::endl;
    }
    void setAge(const int &age)
    {
        this->age = age;
        std::cout << "Hi I'm " << this->name << " and I'm " << this->age << std::endl;
    }    
};

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

    template <typename R, typename... Args>
    R emit(const std::string &eventName, Args &&...args)
    {
        const std::any &eventCallback = events[eventName];
        return std::any_cast<std::function<R(Args...)>>(eventCallback)(std::forward<Args>(std::move(args))...);
    }

    ~EventEmitter() {}
};

void fun1()
{
    std::cout << "function fun1 with no arguments" << std::endl;
}

double fun2(int i)
{
    double r = double(i);

    std::cout << "function fun2 with one argument " << i << std::endl;

    return r;
}

double fun3(int x, int y)
{
    double r = double(x + y);

    std::cout << "function fun3 with two arguments " << x << " and " << y << std::endl;

    return r;
}

int main(int argc, char *argv[])
{
    
    EventEmitter e;

    // No argument

    e.on("fun1", std::function<void(void)>(fun1));
    e.emit<void>("fun1");

    // 1 argument

    e.on("fun2", std::function<double(int)>(fun2));
    e.emit<double, int>("fun2", 1);

    // 2 arguments

    e.on("fun3", std::function<double(int, int)>(fun3));
    e.emit<double, int>("fun3", 1, 2);

    // Lambda test

    e.on("lambda", std::function<void(void)>([](){
        std::cout << "Hello, this is lambda test" << std::endl;
    }));
    e.emit<void>("lambda");

    // Class member test 1

    Person p = Person("Peter");
    
    e.on("member1", std::function<void(void)>(std::bind(&Person::introduce, p)));
    e.emit<void>("member1");

    // Class member test 2

    e.on("member2", std::function<void(void)>(std::bind(&Person::setAge, p, 36)));
    e.emit<void>("member2");    
    
    // Class member test 3

    e.on("member3", std::function<void(int)>([&](int age){
        p.setAge(age);
    }));
    e.emit<void>("member3", 36);

    return 0;
}