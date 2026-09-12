#pragma once

#include "SettingsPage.g.h"

namespace winrt::App2::implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage>
    {
        SettingsPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void StartSyncAnimation();
        void StopSyncAnimation();

        winrt::fire_and_forget RunSyncAsync();

        void SyncButton_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);

        void BackdropComboBox_SelectionChanged(
            winrt::Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);

        void NotificationsComboBox_SelectionChanged(
            winrt::Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);

    private:
        bool m_isSyncing = false;
    };
}


namespace winrt::App2::factory_implementation
{
    struct SettingsPage :
        SettingsPageT<SettingsPage, implementation::SettingsPage>
    {
    };
}