#pragma once

#include "RulesPage.g.h"

#include <string>

namespace winrt::App2::implementation
{
    struct RulesPage : RulesPageT<RulesPage>
    {
        RulesPage();

        void DeleteRule_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);

        winrt::fire_and_forget AddRule_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);

        void RemoveAllRules_Click(
            winrt::Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        winrt::hstring m_pendingFolderPath;

        void ReloadRules();

        winrt::fire_and_forget OpenRuleDialogAsync(
            bool isEdit,
            std::string originalPath,
            int originalExpiration,
            std::string originalFiletypes);

        winrt::fire_and_forget PickFolderForDialogAsync(
            Microsoft::UI::Xaml::Controls::TextBlock pickedText,
            Microsoft::UI::Xaml::Controls::ContentDialog dialog,
            Microsoft::UI::Xaml::Controls::TextBlock errorText);
    };
}

namespace winrt::App2::factory_implementation
{
    struct RulesPage :
        RulesPageT<RulesPage, implementation::RulesPage>
    {
    };
}