#pragma once

#include "HomePage.g.h"

namespace winrt::App2::implementation
{
    struct HomePage : HomePageT<HomePage>
    {
        HomePage();

        void OnNavigatedTo(
            Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::App2::factory_implementation
{
    struct HomePage : HomePageT<HomePage, implementation::HomePage>
    {
    };
}