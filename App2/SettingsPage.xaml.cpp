#include "pch.h"
#include "RetentionControl.h"
#include "SettingsPage.xaml.h"
#include "App.xaml.h"

#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <winrt/Microsoft.Windows.AppNotifications.Builder.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.Storage.h>

#include <atomic>
#include <string>
#include <exception>
#include <ctime>
#include <cwchar>

#if __has_include("SettingsPage.g.cpp")
#include "SettingsPage.g.cpp"
#endif

using namespace winrt::Microsoft::Windows::AppNotifications;
using namespace winrt::Microsoft::Windows::AppNotifications::Builder;

namespace {
    constexpr wchar_t SyncNotificationTag[] = L"sync-progress";
    constexpr wchar_t NotificationsSettingKey[] = L"NotificationsEnabled";
    constexpr wchar_t BackdropSettingKey[] = L"Backdrop";
    constexpr wchar_t LastCheckedSettingKey[] = L"LastChecked";
    std::atomic<uint32_t> SyncSequence{ 0 };

    bool LoadNotificationsEnabled() {
        try {
            auto values = winrt::Windows::Storage::ApplicationData::Current().LocalSettings().Values();
            if (!values.HasKey(NotificationsSettingKey)) {
                return true;
            }
            return winrt::unbox_value<bool>(values.Lookup(NotificationsSettingKey));
        }
        catch (...) {
            return true;
        }
    }

    std::atomic_bool& NotificationsEnabledState() {
        static std::atomic_bool enabled{ LoadNotificationsEnabled() };
        return enabled;
    }

    void SaveNotificationsEnabled(bool enabled) {
        try {
            auto values = winrt::Windows::Storage::ApplicationData::Current().LocalSettings().Values();
            values.Insert(NotificationsSettingKey, winrt::box_value(enabled));
        }
        catch (...) {
        }
    }

    winrt::hstring LoadBackdropSetting() {
        try {
            auto values = winrt::Windows::Storage::ApplicationData::Current().LocalSettings().Values();
            if (!values.HasKey(BackdropSettingKey)) {
                return L"Mica";
            }
            return winrt::unbox_value<winrt::hstring>(values.Lookup(BackdropSettingKey));
        }
        catch (...) {
            return L"Mica";
        }
    }

    void SaveBackdropSetting(winrt::hstring const& backdrop) {
        try {
            auto values = winrt::Windows::Storage::ApplicationData::Current().LocalSettings().Values();
            values.Insert(BackdropSettingKey, winrt::box_value(backdrop));
        }
        catch (...) {
        }
    }

    winrt::hstring CreateLastCheckedText() {
        std::tm timeNow = getTimeNow();
        wchar_t buffer[128]{};
        swprintf_s(
            buffer,
            _countof(buffer),
            L"Last checked: %02d/%02d/%04d %02d:%02d",
            timeNow.tm_mday,
            timeNow.tm_mon + 1,
            timeNow.tm_year + 1900,
            timeNow.tm_hour,
            timeNow.tm_min
        );
        return winrt::hstring{ buffer };
    }

    winrt::hstring LoadLastChecked() {
        try {
            auto values = winrt::Windows::Storage::ApplicationData::Current().LocalSettings().Values();
            if (!values.HasKey(LastCheckedSettingKey)) {
                return L"Last checked: Never";
            }
            return winrt::unbox_value<winrt::hstring>(values.Lookup(LastCheckedSettingKey));
        }
        catch (...) {
            return L"Last checked: Never";
        }
    }

    void SaveLastChecked(winrt::hstring const& text) {
        try {
            auto values = winrt::Windows::Storage::ApplicationData::Current().LocalSettings().Values();
            values.Insert(LastCheckedSettingKey, winrt::box_value(text));
        }
        catch (...) {
        }
    }

    winrt::fire_and_forget RemoveSyncNotificationAsync() {
        try {
            co_await AppNotificationManager::Default()
                .RemoveByTagAsync(winrt::hstring{ SyncNotificationTag });
        }
        catch (...) {
        }
    }
}

void UpdateSyncProgress(int percentage, winrt::hstring const& title, winrt::hstring const& status) {
    if (!NotificationsEnabledState().load()) {
        return;
    }
    if (percentage < 0) {
        percentage = 0;
    }
    if (percentage > 100) {
        percentage = 100;
    }
    AppNotificationProgressData data(++SyncSequence);
    data.Title(title);
    data.Value(static_cast<double>(percentage) / 100.0);
    data.ValueStringOverride(winrt::hstring(std::to_wstring(percentage) + L"%"));
    data.Status(status);
    AppNotificationManager::Default().UpdateAsync(data, SyncNotificationTag);
}

namespace winrt::App2::implementation {

    SettingsPage::SettingsPage() {
        InitializeComponent();

        //Restore last checked time

        LastCheckedTextBlock().Text(LoadLastChecked());

        //Restore notification setting

        NotificationsComboBox().SelectedIndex(NotificationsEnabledState().load() ? 0 : 1);

        //Restore backdrop setting
        auto backdrop = LoadBackdropSetting();
        if (backdrop == L"Acrylic") {
            BackdropComboBox().SelectedIndex(1);
        }
        else {
            BackdropComboBox().SelectedIndex(0);
        }
    }

    int32_t SettingsPage::MyProperty() {
        return 0;
    }
    void SettingsPage::MyProperty(int32_t value) {
        (void)value;
    }

    void SettingsPage::StartSyncAnimation() {
        m_isSyncing = true;
        butt().IsEnabled(false);
        SyncSpin().Begin();
    }

    void SettingsPage::StopSyncAnimation() {
        SyncSpin().Stop();
        m_isSyncing = false;
        butt().IsEnabled(true);
    }

    winrt::fire_and_forget SettingsPage::RunSyncAsync() {
        //keeps SettingsPage alive until this coroutine completes.
        auto lifetime = get_strong();
        //saves while still on UI thread.
        auto dispatcher = DispatcherQueue();
        bool syncFailed = false;

        co_await winrt::resume_background();
        try {
            processAllRules();
        }
        catch (std::exception const&) {
            syncFailed = true;
        }
        catch (...) {
            syncFailed = true;
        }

        if (syncFailed) {
            UpdateSyncProgress(0, L"Sync failed", L"An error occurred during synchronization.");
        }

        co_await wil::resume_foreground(dispatcher);

        if (!syncFailed) {
            auto lastChecked = CreateLastCheckedText();
            LastCheckedTextBlock().Text(lastChecked);
            SaveLastChecked(lastChecked);
        }
        StopSyncAnimation();
    }

    void SettingsPage::SyncButton_Click(winrt::Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&) {
        if (m_isSyncing) {
            return;
        }

        StartSyncAnimation();

        if (NotificationsEnabledState().load()) {
            try {
                auto notification =
                    AppNotificationBuilder()
                    .AddText(L"Retention Control")
                    .AddProgressBar(AppNotificationProgressBar().BindTitle().BindValue().BindValueStringOverride().BindStatus())
                    .SetTag(SyncNotificationTag)
                    .BuildNotification();
                AppNotificationProgressData data(++SyncSequence);
                data.Title(L"Synchronizing files");
                data.Value(0.0);
                data.ValueStringOverride(L"0%");
                data.Status(L"Sync initiated...");
                notification.Progress(data);
                AppNotificationManager::Default().Show(notification);
            }
            catch (...) {
                //Notification errors = not crash app.
            }
        }

        RunSyncAsync();
    }

    void SettingsPage::NotificationsComboBox_SelectionChanged(winrt::Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&) {
        int32_t selectedIndex = NotificationsComboBox().SelectedIndex();
        if (selectedIndex < 0) {
            return;
        }
        bool enabled = selectedIndex == 0;
        NotificationsEnabledState().store(enabled);
        SaveNotificationsEnabled(enabled);
        if (!enabled) {
            RemoveSyncNotificationAsync();
        }
    }

    void SettingsPage::BackdropComboBox_SelectionChanged(winrt::Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&) {
        int32_t selectedIndex = BackdropComboBox().SelectedIndex();
        if (selectedIndex < 0) {
            return;
        }
        auto window = App::GetMainWindow();
        if (!window) {
            return;
        }
        switch (selectedIndex) {
        case 0: {
            SaveBackdropSetting(L"Mica");
            window.SystemBackdrop(Microsoft::UI::Xaml::Media::MicaBackdrop{});
            break;
        }
        case 1: {
            SaveBackdropSetting(L"Acrylic");
            window.SystemBackdrop(Microsoft::UI::Xaml::Media::DesktopAcrylicBackdrop{});
            break;
        }
        default:
            break;
        }
    }
}