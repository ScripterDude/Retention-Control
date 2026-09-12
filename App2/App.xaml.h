#pragma once

#include "App.xaml.g.h"

namespace winrt::App2::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(
            Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

        static Microsoft::UI::Xaml::Window GetMainWindow()
        {
            return m_window;
        }

    private:
        inline static Microsoft::UI::Xaml::Window m_window{ nullptr };
    };
}