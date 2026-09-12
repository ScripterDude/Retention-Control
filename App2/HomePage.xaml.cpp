#include "pch.h"
#include "HomePage.xaml.h"
#include "RetentionControl.h"

#if __has_include("HomePage.g.cpp")
#include "HomePage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Navigation;

namespace winrt::App2::implementation
{
    HomePage::HomePage()
    {
        InitializeComponent();
    }

    void HomePage::OnNavigatedTo(
        NavigationEventArgs const& e)
    {

        //update numerical values each time home is accessed.
        std::vector<file> loggedFiles = getRemovedFiles();
        std::vector<Rule> rules = getRules();
        RemovedFilesCountText().Text(
            std::to_wstring(loggedFiles.size()));

        ActiveRulesCountText().Text(
            std::to_wstring(rules.size()));

        __super::OnNavigatedTo(e);
    }

    int32_t HomePage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void HomePage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}