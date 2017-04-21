#include "../include/Interface.h"

#include <cassert>

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

    class InheretedClass : public TestInterface
    {
        void TestMethod() override {};
    };


    assert(IsInterface<TestInterface>::Value && "TestInterface is not an interface!");
    assert(IsInterface<PlainClass>::Value == false && "PlainClass is an interface!");
    assert(IsInterface<InheretedClass>::Value == false && "InheretedClass is an interface!");
}

int main()
{
    TestInterface();

    return 0;
}