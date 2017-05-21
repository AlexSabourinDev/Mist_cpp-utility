#include "../include/Interface.h"
#include "../include/Singleton.h"
#include "../include/RingBuffer.h"

#include <cassert>
#include <iostream>
#include <string>

void Pause()
{
	std::cout << "Press Any Key" << std::endl;
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

    class InheretedClass : public Mist::Interface<TestInterface>
    {
        void TestMethod() override {};
    };


    assert(Mist::IsInterface<TestInterface>::Value && "TestInterface is not an interface!");
    assert(Mist::IsInterface<PlainClass>::Value == false && "PlainClass is an interface!");
    assert(Mist::IsInterface<InheretedClass>::Value == false && "InheretedClass is an interface!");
}

void TestSingleton()
{
    class SingletonTest : public Mist::Singleton<SingletonTest>
    {
    public:
        void Print()
        {
            std::cout << "Singleton Print, count: " + std::to_string(SingletonTest::cSingletonCount) << std::endl;
        }
    };

    SingletonTest::Instance()->Print();
}

void TestRingBuffer()
{
	std::cout << "RingBuffer Test" << std::endl;

	Mist::RingBuffer<int, 5> buffer;
	for (int i = 0; i < 5; i++)
	{
		assert(buffer.TryWrite(10));
	}
	for (int i = 0; i < 3; i++)
	{
		int result = 0;
		assert(buffer.TryRead(&result));

		std::cout << result << std::endl;
	}
}

int main()
{
    TestInterface();
    TestSingleton();
	TestRingBuffer();

	Pause();
    return 0;
}