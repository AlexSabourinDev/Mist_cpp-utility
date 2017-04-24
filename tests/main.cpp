#include "../include/Interface.h"
#include "../include/Singleton.h"

#include <cassert>
#include <iostream>
#include <string>

void Pause()
{
    char c;
    std::cin >> c;
}

void TestInterface()
{
    class TestInterface
    {
        virtual void TestMethod() = 0;
    };

    class PlainClass
    {
        int m_Data;
    };

    class InheretedClass : public Utility::Interface<TestInterface>
    {
        void TestMethod() override {};
    };


    assert(Utility::IsInterface<TestInterface>::Value && "TestInterface is not an interface!");
    assert(Utility::IsInterface<PlainClass>::Value == false && "PlainClass is an interface!");
    assert(Utility::IsInterface<InheretedClass>::Value == false && "InheretedClass is an interface!");
}

void TestSingleton()
{
    class SingletonTest : public Utility::Singleton<SingletonTest>
    {
    public:
        void Print()
        {
            std::cout << "Singleton Print, count: " + std::to_string(SingletonTest::SingletonCount) << std::endl;
        }
    };

    SingletonTest::Instance()->Print();
    Pause();
}

int main()
{
    TestInterface();
    TestSingleton();

    return 0;
}