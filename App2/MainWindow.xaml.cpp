#include "pch.h"

#include "MainWindow.xaml.h"
#include "RulesPage.xaml.h"
#include "HomePage.xaml.h"
#include "SettingsPage.xaml.h"
#include "RetentionControl.h"

#include <cstdlib>
#include <string>
#include <filesystem>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Comctl32.lib")

#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;

namespace
{
    std::filesystem::path GetApplicationDirectory()
    {
        wchar_t modulePath[32768]{};

        DWORD length =
            GetModuleFileNameW(
                nullptr,
                modulePath,
                static_cast<DWORD>(
                    std::size(modulePath)
                    )
            );

        if (
            length == 0 ||
            length >= std::size(modulePath)
            )
        {
            return {};
        }

        return std::filesystem::path(
            modulePath
        ).parent_path();
    }

    HICON LoadRetentionControlIcon()
    {
        std::filesystem::path appDirectory =
            GetApplicationDirectory();

        if (appDirectory.empty())
        {
            return nullptr;
        }

        std::filesystem::path iconPath =
            appDirectory /
            L"Assets" /
            L"RetentionControl.ico";

        if (
            !std::filesystem::exists(
                iconPath
            )
            )
        {
            OutputDebugStringW(
                L"RetentionControl.ico was not found at:\n"
            );

            OutputDebugStringW(
                iconPath.c_str()
            );

            OutputDebugStringW(
                L"\n"
            );

            return nullptr;
        }

        int iconWidth =
            GetSystemMetrics(
                SM_CXSMICON
            );

        int iconHeight =
            GetSystemMetrics(
                SM_CYSMICON
            );

        HICON icon =
            static_cast<HICON>(
                LoadImageW(
                    nullptr,
                    iconPath.c_str(),
                    IMAGE_ICON,
                    iconWidth,
                    iconHeight,
                    LR_LOADFROMFILE
                )
                );

        if (!icon)
        {
            OutputDebugStringW(
                L"Failed to load RetentionControl.ico\n"
            );
        }

        return icon;
    }
}

namespace winrt::App2::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();

        const char* appData =
            std::getenv(
                "APPDATA"
            );

        if (appData)
        {
            std::string fullpath =
                std::string(
                    appData
                ) +
                "\\Retention Control";

            if (
                !setupExists(
                    fullpath
                )
                )
            {
                setup(
                    fullpath,
                    false
                );
            }
        }

        for (
            auto const& menuItem :
            nvSample().MenuItems()
            )
        {
            if (
                auto navItem =
                menuItem.try_as<
                Microsoft::UI::Xaml::Controls::
                NavigationViewItem
                >()
                )
            {
                auto tag =
                    winrt::unbox_value_or<
                    winrt::hstring
                    >(
                        navItem.Tag(),
                        L""
                    );

                if (
                    tag ==
                    L"home"
                    )
                {
                    nvSample().
                        SelectedItem(
                            navItem
                        );

                    break;
                }
            }
        }

        AppWindow().Resize(
            winrt::Windows::Graphics::
            SizeInt32{
                777,
                507
            }
        );

        ExtendsContentIntoTitleBar(
            true
        );

        SetTitleBar(
            AppTitleBar()
        );

        InitializeTray();

        AppWindow().Closing(
            {
                this,
                &MainWindow::AppWindow_Closing
            }
        );
    }

    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(
        int32_t
    )
    {
        throw hresult_not_implemented();
    }

    void MainWindow::InitializeTray()
    {
        auto windowNative{
            this->m_inner.as<
                ::IWindowNative
            >()
        };

        check_hresult(
            windowNative->
            get_WindowHandle(
                &m_hwnd
            )
        );

        SetWindowSubclass(
            m_hwnd,
            TraySubclassProc,
            TRAY_SUBCLASS_ID,
            reinterpret_cast<
            DWORD_PTR
            >(this)
        );

        AddTrayIcon();
    }

    void MainWindow::AddTrayIcon()
    {
        ZeroMemory(
            &m_trayIcon,
            sizeof(
                m_trayIcon
                )
        );

        m_trayIcon.cbSize =
            sizeof(
                NOTIFYICONDATAW
                );

        m_trayIcon.hWnd =
            m_hwnd;

        m_trayIcon.uID =
            1;

        m_trayIcon.uFlags =
            NIF_MESSAGE |
            NIF_ICON |
            NIF_TIP;

        m_trayIcon.uCallbackMessage =
            WM_TRAYICON;

        m_trayIcon.hIcon =
            LoadRetentionControlIcon();

        if (
            !m_trayIcon.hIcon
            )
        {
            HICON fallbackIcon =
                LoadIconW(
                    nullptr,
                    MAKEINTRESOURCEW(
                        32512
                    )
                );

            if (fallbackIcon)
            {
                m_trayIcon.hIcon =
                    CopyIcon(
                        fallbackIcon
                    );
            }
        }

        wcscpy_s(
            m_trayIcon.szTip,
            L"Retention Control"
        );

        if (
            Shell_NotifyIconW(
                NIM_ADD,
                &m_trayIcon
            )
            )
        {
            m_trayIcon.uVersion =
                NOTIFYICON_VERSION_4;

            Shell_NotifyIconW(
                NIM_SETVERSION,
                &m_trayIcon
            );
        }
    }

    void MainWindow::RemoveTrayIcon()
    {
        if (
            m_trayIcon.cbSize != 0
            )
        {
            Shell_NotifyIconW(
                NIM_DELETE,
                &m_trayIcon
            );

            if (
                m_trayIcon.hIcon
                )
            {
                DestroyIcon(
                    m_trayIcon.hIcon
                );

                m_trayIcon.hIcon =
                    nullptr;
            }

            m_trayIcon.cbSize =
                0;
        }
    }

    void MainWindow::AppWindow_Closing(
        winrt::Microsoft::UI::Windowing::
        AppWindow const&,
        winrt::Microsoft::UI::Windowing::
        AppWindowClosingEventArgs const& args
    )
    {
        if (
            !m_exitRequested
            )
        {
            args.Cancel(
                true
            );

            ShowWindow(
                m_hwnd,
                SW_HIDE
            );
        }
    }

    void MainWindow::ShowFromTray()
    {
        ShowWindow(
            m_hwnd,
            SW_SHOW
        );

        if (
            IsIconic(
                m_hwnd
            )
            )
        {
            ShowWindow(
                m_hwnd,
                SW_RESTORE
            );
        }

        SetForegroundWindow(
            m_hwnd
        );
    }

    void MainWindow::ShowTrayMenu()
    {
        POINT cursorPosition{};

        GetCursorPos(
            &cursorPosition
        );

        HMENU menu =
            CreatePopupMenu();

        if (!menu)
        {
            return;
        }

        AppendMenuW(
            menu,
            MF_STRING,
            ID_TRAY_OPEN,
            L"Open Retention Control"
        );

        AppendMenuW(
            menu,
            MF_SEPARATOR,
            0,
            nullptr
        );

        AppendMenuW(
            menu,
            MF_STRING,
            ID_TRAY_EXIT,
            L"Exit"
        );

        SetForegroundWindow(
            m_hwnd
        );

        UINT command =
            TrackPopupMenu(
                menu,
                TPM_RETURNCMD |
                TPM_RIGHTBUTTON,
                cursorPosition.x,
                cursorPosition.y,
                0,
                m_hwnd,
                nullptr
            );

        DestroyMenu(
            menu
        );

        PostMessageW(
            m_hwnd,
            WM_NULL,
            0,
            0
        );

        switch (command)
        {
        case ID_TRAY_OPEN:

            ShowFromTray();

            break;

        case ID_TRAY_EXIT:

            m_exitRequested =
                true;

            RemoveTrayIcon();

            PostMessageW(
                m_hwnd,
                WM_CLOSE,
                0,
                0
            );

            break;

        default:

            break;
        }
    }

    LRESULT CALLBACK
        MainWindow::TraySubclassProc(
            HWND hwnd,
            UINT message,
            WPARAM wParam,
            LPARAM lParam,
            UINT_PTR subclassId,
            DWORD_PTR refData
        )
    {
        auto self =
            reinterpret_cast<
            MainWindow*
            >(
                refData
                );

        if (!self)
        {
            return DefSubclassProc(
                hwnd,
                message,
                wParam,
                lParam
            );
        }

        if (
            message ==
            WM_TRAYICON
            )
        {
            UINT trayEvent =
                LOWORD(
                    lParam
                );

            switch (trayEvent)
            {
            case WM_LBUTTONUP:
            case WM_LBUTTONDBLCLK:
            case NIN_SELECT:
            case NIN_KEYSELECT:

                self->
                    ShowFromTray();

                return 0;

            case WM_RBUTTONUP:
            case WM_CONTEXTMENU:

                self->
                    ShowTrayMenu();

                return 0;

            default:

                break;
            }
        }

        return DefSubclassProc(
            hwnd,
            message,
            wParam,
            lParam
        );
    }

    MainWindow::~MainWindow()
    {
        RemoveTrayIcon();

        if (
            m_hwnd
            )
        {
            RemoveWindowSubclass(
                m_hwnd,
                TraySubclassProc,
                TRAY_SUBCLASS_ID
            );
        }
    }

    void MainWindow::NavView_SelectionChanged(
        winrt::Microsoft::UI::Xaml::Controls::
        NavigationView const& sender,
        winrt::Microsoft::UI::Xaml::Controls::
        NavigationViewSelectionChangedEventArgs const& args
    )
    {
        auto item =
            args.SelectedItem()
            .try_as<
            winrt::Microsoft::UI::Xaml::
            Controls::NavigationViewItem
            >();

        if (!item)
        {
            return;
        }

        auto tag =
            winrt::unbox_value_or<
            winrt::hstring
            >(
                item.Tag(),
                L""
            );

        if (
            tag ==
            L"home"
            )
        {
            nvSample().Header(
                nullptr
            );

            contentFrame().Navigate(
                winrt::xaml_typename<
                winrt::App2::HomePage
                >()
            );

            return;
        }

        if (
            tag ==
            L"rules"
            )
        {
            nvSample().Header(
                NavHeaderContainer()
            );

            NavHeaderContainer().
                Visibility(
                    Microsoft::UI::Xaml::
                    Visibility::Visible
                );

            NavHeader().Text(
                L"Rules"
            );

            NavSubHeader().Text(
                L"Rules automatically remove old files "
                L"from a selected folder when they pass "
                L"the expiration period you set."
            );

            contentFrame().Navigate(
                winrt::xaml_typename<
                winrt::App2::RulesPage
                >()
            );

            return;
        }

        if (
            args.IsSettingsSelected()
            )
        {
            nvSample().Header(
                NavHeaderContainer()
            );

            NavHeaderContainer().
                Visibility(
                    Microsoft::UI::Xaml::
                    Visibility::Visible
                );

            NavHeader().Text(
                L"Settings"
            );

            NavSubHeader().Text(
                L"Configure synchronization, "
                L"notifications, and appearance."
            );

            contentFrame().Navigate(
                winrt::xaml_typename<
                winrt::App2::SettingsPage
                >()
            );

            return;
        }
    }
}