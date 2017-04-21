#pragma once

#include <type_traits>

// Type trait that determines if the class is a pure interface with no associated data
template< typename InterfaceType >
class IsInterface
{
private:
    // BaseInterface is utilized to test the size of this class against the class of the interface class
    class BaseInterface
    {
        virtual void DummyMethod() = 0;
    };

public:
    static constexpr bool Value = sizeof(InterfaceType) == sizeof(BaseInterface) && std::is_abstract<InterfaceType>::value;
};
