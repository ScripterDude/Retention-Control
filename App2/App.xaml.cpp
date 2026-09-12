#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"

#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.Storage.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;


// ============================================================
// APP SETTINGS HELPERS
// ============================================================

namespace
{
    constexpr wchar_t BackdropSettingKey[] =
        L"Backdrop";


    // ========================================================
    // LOAD SAVED BACKDROP
    //
    // Defaults to Mica for first launch or if settings cannot
    // be read.
    // ========================================================

    winrt::hstring LoadBackdropSetting()
    {
        try
        {
            auto values =
                winrt::Windows::Storage::ApplicationData::Current()
                .LocalSettings()
                .Values();


            if (!values.HasKey(BackdropSettingKey))
            {
                return L"Mica";
            }


            return winrt::unbox_value<winrt::hstring>(
                values.Lookup(BackdropSettingKey)
            );
        }
        catch (...)
        {
            return L"Mica";
        }
    }
}


// ============================================================
// APPLICATION
// ============================================================

namespace winrt::App2::implementation
{

    // ========================================================
    // CONSTRUCTOR
    // ========================================================

    App::App()
    {
        // Xaml objects should not call InitializeComponent
        // during construction.
        //
        // See:
        // https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent


#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION

        UnhandledException(
            [](
                IInspectable const&,
                UnhandledExceptionEventArgs const& e)
            {
                if (IsDebuggerPresent())
                {
                    auto errorMessage =
                        e.Message();

                    __debugbreak();
                }
            }
        );

#endif
    }


    // ========================================================
    // APPLICATION LAUNCH
    // ========================================================

    void App::OnLaunched(
        [[maybe_unused]]
    LaunchActivatedEventArgs const& e)
    {
        // ----------------------------------------------------
        // CREATE MAIN WINDOW
        // ----------------------------------------------------

        m_window =
            make<MainWindow>();


        // ----------------------------------------------------
        // RESTORE SAVED BACKDROP
        // ----------------------------------------------------

        auto backdrop =
            LoadBackdropSetting();


        if (backdrop == L"Acrylic")
        {
            m_window.SystemBackdrop(
                Microsoft::UI::Xaml::Media::
                DesktopAcrylicBackdrop{}
            );
        }
        else
        {
            // Default / saved Mica.
            m_window.SystemBackdrop(
                Microsoft::UI::Xaml::Media::
                MicaBackdrop{}
            );
        }


        // ----------------------------------------------------
        // SHOW WINDOW
        // ----------------------------------------------------

        m_window.Activate();
    }

}