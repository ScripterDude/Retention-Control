#pragma once

#include "BlankPage.g.h"

namespace winrt::App2::implementation
{
    struct BlankPage : BlankPageT<BlankPage>
    {
        BlankPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::App2::factory_implementation
{
    struct BlankPage : BlankPageT<BlankPage, implementation::BlankPage>
    {
    };
}
