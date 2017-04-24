#pragma once

#include <type_traits>

namespace Utility
{
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
        using Type = InterfaceType;

        static constexpr bool Value = sizeof(InterfaceType) == sizeof(BaseInterface) && std::is_abstract<InterfaceType>::value;
    };

    // Interface trait that requires that the class being inherited is a pure interface with no data
    template< typename InterfaceType >
    class Interface : public InterfaceType
    {
        static_assert(IsInterface<InterfaceType>::Value, "The interface you are trying to implement does not meet the interface criteria! It must have no data members.");

    public:
        using Type = InterfaceType;
    };
}
