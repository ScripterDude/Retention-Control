#pragma once

#include "MainWindow.g.h"

#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>

namespace winrt::App2::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        ~MainWindow();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void NavView_SelectionChanged(
            winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender,
            winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args
        );

    private:
        HWND m_hwnd{ nullptr };

        NOTIFYICONDATAW m_trayIcon{};

        bool m_exitRequested{ false };

        static constexpr UINT WM_TRAYICON =
            WM_APP + 1;

        static constexpr UINT ID_TRAY_OPEN =
            1001;

        static constexpr UINT ID_TRAY_EXIT =
            1002;

        static constexpr UINT_PTR TRAY_SUBCLASS_ID =
            1;

        void InitializeTray();

        void AddTrayIcon();

        void RemoveTrayIcon();

        void ShowFromTray();

        void ShowTrayMenu();

        void AppWindow_Closing(
            winrt::Microsoft::UI::Windowing::AppWindow const& sender,
            winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs const& args
        );

        static LRESULT CALLBACK TraySubclassProc(
            HWND hwnd,
            UINT message,
            WPARAM wParam,
            LPARAM lParam,
            UINT_PTR subclassId,
            DWORD_PTR refData
        );
    };
}

namespace winrt::App2::factory_implementation
{
    struct MainWindow :
        MainWindowT<
        MainWindow,
        implementation::MainWindow
        >
    {
    };
}