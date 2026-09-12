#include "pch.h"
#include "RulesPage.xaml.h"

#include <winrt/Microsoft.Windows.Storage.Pickers.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Content.h>

#include "RetentionControl.h"

#include <cmath>
#include <filesystem>
#include <limits>
#include <string>
#include <vector>

#if __has_include("RulesPage.g.cpp")
#include "RulesPage.g.cpp"
#endif

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Microsoft::UI::Content;
using namespace winrt::Microsoft::Windows::Storage::Pickers;

namespace winrt::App2::implementation
{
    RulesPage::RulesPage()
    {
        InitializeComponent();

        try
        {
            ReloadRules();
        }
        catch (winrt::hresult_error const& e)
        {
            OutputDebugStringW(L"ReloadRules crashed: ");
            OutputDebugStringW(e.message().c_str());
            OutputDebugStringW(L"\n");
        }
    }

    void RulesPage::ReloadRules()
    {
        RulesList().Items().Clear();

        std::vector<Rule> rules = getRules();

        EmptyText().Visibility(
            rules.empty()
            ? Visibility::Visible
            : Visibility::Collapsed
        );

        for (const Rule& rule : rules)
        {
            winrt::hstring pathHString =
                winrt::to_hstring(rule.path);

            std::wstring path{
                pathHString.c_str()
            };

            std::filesystem::path fsPath{ path };

            std::wstring folderName =
                fsPath.filename().wstring();

            if (folderName.empty())
            {
                folderName =
                    fsPath.parent_path().filename().wstring();
            }

            Grid row;

            row.Padding(
                Thickness{
                    10, 8, 8, 8
                }
            );

            ColumnDefinition iconColumn;

            iconColumn.Width(
                GridLength{
                    48,
                    GridUnitType::Pixel
                }
            );

            ColumnDefinition infoColumn;

            infoColumn.Width(
                GridLength{
                    1,
                    GridUnitType::Star
                }
            );

            ColumnDefinition fileTypeColumn;

            fileTypeColumn.Width(
                GridLengthHelper::Auto()
            );

            ColumnDefinition daysColumn;

            daysColumn.Width(
                GridLengthHelper::Auto()
            );

            ColumnDefinition editColumn;

            editColumn.Width(
                GridLengthHelper::Auto()
            );

            row.ColumnDefinitions().Append(
                iconColumn
            );

            row.ColumnDefinitions().Append(
                infoColumn
            );

            row.ColumnDefinitions().Append(
                fileTypeColumn
            );

            row.ColumnDefinitions().Append(
                daysColumn
            );

            row.ColumnDefinitions().Append(
                editColumn
            );

            Border iconBackground;

            iconBackground.Width(36);
            iconBackground.Height(36);

            iconBackground.CornerRadius(
                CornerRadiusHelper::FromUniformRadius(
                    6.0
                )
            );

            iconBackground.HorizontalAlignment(
                HorizontalAlignment::Center
            );

            iconBackground.VerticalAlignment(
                VerticalAlignment::Center
            );

            FontIcon folderIcon;

            folderIcon.Glyph(
                L"\xE8B7"
            );

            folderIcon.FontSize(
                18
            );

            iconBackground.Child(
                folderIcon
            );

            Grid::SetColumn(
                iconBackground,
                0
            );

            StackPanel infoPanel;

            infoPanel.Spacing(
                2
            );

            infoPanel.VerticalAlignment(
                VerticalAlignment::Center
            );

            TextBlock folderText;

            folderText.Text(
                winrt::hstring{
                    folderName
                }
            );

            folderText.FontSize(
                15
            );

            folderText.FontWeight(
                winrt::Windows::UI::Text::
                FontWeights::SemiBold()
            );

            folderText.TextTrimming(
                TextTrimming::CharacterEllipsis
            );

            TextBlock pathText;

            pathText.Text(
                winrt::hstring{
                    path
                }
            );

            pathText.FontSize(
                12
            );

            pathText.Opacity(
                0.65
            );

            pathText.TextTrimming(
                TextTrimming::CharacterEllipsis
            );

            infoPanel.Children().Append(
                folderText
            );

            infoPanel.Children().Append(
                pathText
            );

            Grid::SetColumn(
                infoPanel,
                1
            );

            Border fileTypeBadge;

            fileTypeBadge.Padding(
                Thickness{
                    8, 3, 8, 3
                }
            );

            fileTypeBadge.Margin(
                Thickness{
                    12, 0, 8, 0
                }
            );

            fileTypeBadge.CornerRadius(
                CornerRadiusHelper::FromUniformRadius(
                    10.0
                )
            );

            fileTypeBadge.VerticalAlignment(
                VerticalAlignment::Center
            );

            fileTypeBadge.HorizontalAlignment(
                HorizontalAlignment::Right
            );

            auto badgeBackground =
                Application::Current()
                .Resources()
                .Lookup(
                    winrt::box_value(
                        L"SubtleFillColorSecondaryBrush"
                    )
                )
                .as<Brush>();

            fileTypeBadge.Background(
                badgeBackground
            );

            TextBlock fileTypeText;

            std::string fileTypes =
                rule.filetypes;

            if (
                fileTypes.empty() ||
                fileTypes == "all"
                )
            {
                fileTypeText.Text(
                    L"All files"
                );
            }
            else
            {
                fileTypeText.Text(
                    winrt::to_hstring(
                        fileTypes
                    )
                );
            }

            fileTypeText.FontSize(
                11
            );

            fileTypeText.Opacity(
                0.75
            );

            fileTypeText.MaxWidth(
                100
            );

            fileTypeText.TextTrimming(
                TextTrimming::CharacterEllipsis
            );

            fileTypeBadge.Child(
                fileTypeText
            );

            Grid::SetColumn(
                fileTypeBadge,
                2
            );

            StackPanel daysPanel;

            daysPanel.VerticalAlignment(
                VerticalAlignment::Center
            );

            daysPanel.HorizontalAlignment(
                HorizontalAlignment::Right
            );

            daysPanel.Margin(
                Thickness{
                    16, 0, 12, 0
                }
            );

            TextBlock daysText;

            daysText.Text(
                winrt::hstring{
                    std::to_wstring(
                        rule.expiration
                    ) +
                    L" days"
                }
            );

            daysText.FontSize(
                13
            );

            daysText.FontWeight(
                winrt::Windows::UI::Text::
                FontWeights::SemiBold()
            );

            daysText.HorizontalAlignment(
                HorizontalAlignment::Right
            );

            TextBlock expiresText;

            expiresText.Text(
                L"Expiration"
            );

            expiresText.FontSize(
                11
            );

            expiresText.Opacity(
                0.55
            );

            expiresText.HorizontalAlignment(
                HorizontalAlignment::Right
            );

            daysPanel.Children().Append(
                daysText
            );

            daysPanel.Children().Append(
                expiresText
            );

            Grid::SetColumn(
                daysPanel,
                3
            );

            AppBarButton editButton;

            editButton.Icon(
                SymbolIcon{
                    Symbol::Edit
                }
            );

            editButton.Label(
                L"Edit"
            );

            editButton.VerticalAlignment(
                VerticalAlignment::Center
            );

            Grid::SetColumn(
                editButton,
                4
            );

            auto weakThis =
                get_weak();

            std::string editPath =
                rule.path;

            int editExpiration =
                rule.expiration;

            std::string editFiletypes =
                rule.filetypes;

            editButton.Click(
                [
                    weakThis,
                    editPath,
                    editExpiration,
                    editFiletypes
                ]
                (
                    winrt::Windows::Foundation::
                    IInspectable const&,

                    RoutedEventArgs const&
                    )
                {
                    if (
                        auto self =
                        weakThis.get()
                        )
                    {
                        self->OpenRuleDialogAsync(
                            true,
                            editPath,
                            editExpiration,
                            editFiletypes
                        );
                    }
                }
            );

            row.Children().Append(
                iconBackground
            );

            row.Children().Append(
                infoPanel
            );

            row.Children().Append(
                fileTypeBadge
            );

            row.Children().Append(
                daysPanel
            );

            row.Children().Append(
                editButton
            );

            RulesList().Items().Append(
                row
            );
        }
    }

    void RulesPage::DeleteRule_Click(
        winrt::Windows::Foundation::
        IInspectable const&,

        Microsoft::UI::Xaml::
        RoutedEventArgs const&)
    {
        int index =
            RulesList().SelectedIndex();

        if (index < 0)
        {
            return;
        }

        std::vector<Rule> rules =
            getRules();

        if (
            static_cast<size_t>(index) <
            rules.size()
            )
        {
            removeRule(
                rules[index].path
            );

            ReloadRules();
        }
    }

    winrt::fire_and_forget
        RulesPage::AddRule_Click(
            winrt::Windows::Foundation::
            IInspectable const&,

            Microsoft::UI::Xaml::
            RoutedEventArgs const&)
    {
        OpenRuleDialogAsync(
            false,
            "",
            30,
            "all"
        );

        co_return;
    }

    winrt::fire_and_forget
        RulesPage::OpenRuleDialogAsync(
            bool isEdit,
            std::string originalPath,
            int originalExpiration,
            std::string originalFiletypes)
    {
        auto lifetime =
            get_strong();

        if (isEdit)
        {
            m_pendingFolderPath =
                winrt::to_hstring(
                    originalPath
                );
        }
        else
        {
            m_pendingFolderPath = {};
        }

        ContentDialog dialog;

        dialog.XamlRoot(
            XamlRoot()
        );

        dialog.Title(
            winrt::box_value(
                isEdit
                ? L"Edit rule"
                : L"Add rule"
            )
        );

        dialog.PrimaryButtonText(
            isEdit
            ? L"Save"
            : L"Add"
        );

        dialog.CloseButtonText(
            L"Cancel"
        );

        dialog.DefaultButton(
            ContentDialogButton::Primary
        );

        dialog.IsPrimaryButtonEnabled(
            isEdit
        );

        StackPanel panel;

        panel.Spacing(
            12
        );

        panel.MinWidth(
            400
        );

        TextBlock folderHeader;

        folderHeader.Text(
            L"Folder"
        );

        folderHeader.FontWeight(
            winrt::Windows::UI::Text::
            FontWeights::SemiBold()
        );

        panel.Children().Append(
            folderHeader
        );

        Button pickFolderButton;

        pickFolderButton.Content(
            winrt::box_value(
                isEdit
                ? L"Change folder"
                : L"Pick a folder"
            )
        );

        panel.Children().Append(
            pickFolderButton
        );

        TextBlock pickedFolderText;

        if (isEdit)
        {
            pickedFolderText.Text(
                m_pendingFolderPath
            );

            pickedFolderText.Opacity(
                1.0
            );
        }
        else
        {
            pickedFolderText.Text(
                L"No folder picked"
            );

            pickedFolderText.Opacity(
                0.65
            );
        }

        pickedFolderText.TextWrapping(
            TextWrapping::Wrap
        );

        panel.Children().Append(
            pickedFolderText
        );

        NumberBox expirationBox;

        expirationBox.Header(
            winrt::box_value(
                L"Expiration in days"
            )
        );

        expirationBox.Value(
            isEdit
            ? originalExpiration
            : 30
        );

        expirationBox.Minimum(
            1
        );

        expirationBox.SmallChange(
            1
        );

        expirationBox.LargeChange(
            10
        );

        expirationBox.SpinButtonPlacementMode(
            NumberBoxSpinButtonPlacementMode::Inline
        );

        panel.Children().Append(
            expirationBox
        );

        TextBlock fileTypeHeader;

        fileTypeHeader.Text(
            L"File types"
        );

        fileTypeHeader.FontWeight(
            winrt::Windows::UI::Text::
            FontWeights::SemiBold()
        );

        panel.Children().Append(
            fileTypeHeader
        );

        TextBlock fileTypeValue;

        std::string displayedFiletypes =
            originalFiletypes.empty()
            ? "all"
            : originalFiletypes;

        if (
            displayedFiletypes == "all"
            )
        {
            fileTypeValue.Text(
                L"All files"
            );
        }
        else
        {
            fileTypeValue.Text(
                winrt::to_hstring(
                    displayedFiletypes
                )
            );
        }

        fileTypeValue.Opacity(
            0.65
        );

        panel.Children().Append(
            fileTypeValue
        );

        TextBlock errorText;

        errorText.Visibility(
            Visibility::Collapsed
        );

        errorText.TextWrapping(
            TextWrapping::Wrap
        );

        panel.Children().Append(
            errorText
        );

        dialog.Content(
            panel
        );

        auto weakThis =
            get_weak();

        pickFolderButton.Click(
            [
                weakThis,
                pickedFolderText,
                dialog,
                errorText
            ]
            (
                winrt::Windows::Foundation::
                IInspectable const&,

                RoutedEventArgs const&
                )
            {
                if (
                    auto self =
                    weakThis.get()
                    )
                {
                    self->PickFolderForDialogAsync(
                        pickedFolderText,
                        dialog,
                        errorText
                    );
                }
            }
        );

        dialog.PrimaryButtonClick(
            [
                weakThis,
                expirationBox,
                errorText
            ]
            (
                ContentDialog const&,

                ContentDialogButtonClickEventArgs
                const& args
                )
            {
                auto self =
                    weakThis.get();

                if (!self)
                {
                    args.Cancel(
                        true
                    );

                    return;
                }

                if (
                    self->
                    m_pendingFolderPath.empty()
                    )
                {
                    errorText.Text(
                        L"Please select a folder."
                    );

                    errorText.Visibility(
                        Visibility::Visible
                    );

                    args.Cancel(
                        true
                    );

                    return;
                }

                double value =
                    expirationBox.Value();

                if (
                    !std::isfinite(value) ||
                    value < 1 ||
                    std::floor(value) != value ||
                    value >
                    static_cast<double>(
                        (std::numeric_limits<
                            int>::max)()
                        )
                    )
                {
                    errorText.Text(
                        L"Expiration must be a "
                        L"whole number greater "
                        L"than 0."
                    );

                    errorText.Visibility(
                        Visibility::Visible
                    );

                    args.Cancel(
                        true
                    );

                    return;
                }

                errorText.Visibility(
                    Visibility::Collapsed
                );
            }
        );

        ContentDialogResult result =
            co_await dialog.ShowAsync();

        if (
            result !=
            ContentDialogResult::Primary
            )
        {
            co_return;
        }

        int expirationDays =
            static_cast<int>(
                expirationBox.Value()
                );

        std::string newPath =
            winrt::to_string(
                m_pendingFolderPath
            );

        std::string filetypes =
            originalFiletypes.empty()
            ? "all"
            : originalFiletypes;

        if (isEdit)
        {
            removeRule(
                originalPath
            );

            addRule(
                newPath,
                expirationDays,
                filetypes
            );
        }
        else
        {
            addRule(
                newPath,
                expirationDays,
                filetypes
            );
        }

        ReloadRules();
    }

    void RulesPage::RemoveAllRules_Click(
        winrt::Windows::Foundation::
        IInspectable const&,

        Microsoft::UI::Xaml::
        RoutedEventArgs const&)
    {
        std::vector<Rule> rules =
            getRules();

        for (
            const Rule& rule :
            rules
            )
        {
            removeRule(
                rule.path
            );
        }

        ReloadRules();
    }

    winrt::fire_and_forget
        RulesPage::PickFolderForDialogAsync(
            TextBlock pickedText,
            ContentDialog dialog,
            TextBlock errorText)
    {
        auto lifetime =
            get_strong();

        try
        {
            auto windowId =
                XamlRoot()
                .ContentIslandEnvironment()
                .AppWindowId();

            FolderPicker picker{
                windowId
            };

            picker.Title(
                L"Select folder for rule"
            );

            auto result =
                co_await
                picker.PickSingleFolderAsync();

            if (!result)
            {
                co_return;
            }

            m_pendingFolderPath =
                result.Path();

            pickedText.Text(
                m_pendingFolderPath
            );

            pickedText.Opacity(
                1.0
            );

            errorText.Visibility(
                Visibility::Collapsed
            );

            dialog.IsPrimaryButtonEnabled(
                true
            );
        }
        catch (
            winrt::hresult_error const&
            error
            )
        {
            errorText.Text(
                error.message()
            );

            errorText.Visibility(
                Visibility::Visible
            );

            dialog.IsPrimaryButtonEnabled(
                false
            );
        }
    }
}