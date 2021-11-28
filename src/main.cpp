#include <any>
#include <tuple>
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

typedef std::any EventHandlerCallback;

enum EventHandlerType
{
    ON,
    ONCE
};

class EventEmitter
{
private:
    std::unordered_map<std::string, std::vector<std::tuple<EventHandlerCallback, EventHandlerType>>> events;

public:
    EventEmitter() {}

    void on(const std::string &eventName, const std::any &eventCallback)
    {
        this->events[eventName].emplace_back(std::make_tuple(eventCallback, ON));
    }

    void once(const std::string &eventName, const std::any &eventCallback)
    {
        this->events[eventName].emplace_back(std::make_tuple(eventCallback, ONCE));
    }    

    template <typename... Args>
    void emit(const std::string &eventName, Args &&...args)
    {
        if (this->events.find(eventName) != this->events.end())
        {
            
            std::vector<std::tuple<EventHandlerCallback, EventHandlerType>> &eventHandlers = events[eventName];
            
            for (std::vector<std::tuple<EventHandlerCallback, EventHandlerType>>::iterator it = eventHandlers.begin(); it != eventHandlers.end();) {

                std::any_cast<std::function<void(Args...)>>(std::get<0>(*it))(std::forward<Args>(std::move(args))...);

                if (std::get<1>(*it) == ONCE) {
                    it = eventHandlers.erase(it);
                } else {
                    ++it;
                }

            }

        }
        
    }

    ~EventEmitter() {}
};

void fun1()
{
    std::cout << "function fun1 with no arguments" << std::endl;
}

void fun2(int i)
{
    double r = double(i);

    std::cout << "function fun2 with one argument " << i << std::endl;

}

void fun3(int x, int y)
{
    double r = double(x + y);

    std::cout << "function fun3 with two arguments " << x << " and " << y << std::endl;

}

int main(int argc, char *argv[])
{

    EventEmitter e;
    
    // No argument & once testing

    e.once("fun1", std::function<void(void)>(fun1));
    e.on("fun1", std::function<void(void)>(fun1));
    e.emit("fun1");
    e.emit("fun1");

    // 1 argument

    e.on("fun2", std::function<void(int)>(fun2));
    e.emit<int>("fun2", 1);

    // 2 arguments

    e.on("fun3", std::function<void(int, int)>(fun3));
    e.emit<int>("fun3", 1, 2);

    // Lambda test

    e.on("lambda", std::function<void(void)>([]() { 
        std::cout << "Hello, this is lambda test" << std::endl; 
    }));
    e.emit("lambda");

    // Class member test 1

    Person p = Person("Peter");

    e.on("member1", std::function<void(void)>(std::bind(&Person::introduce, p)));
    e.emit("member1");

    // Class member test 2

    e.on("member2", std::function<void(void)>(std::bind(&Person::setAge, p, 36)));
    e.emit("member2");

    // Class member test 3

    e.on("member3", std::function<void(int)>([&](int &&age){ 
        p.setAge(age); 
    }));
    e.emit("member3", 36);

    return 0;
}