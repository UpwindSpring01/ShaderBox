using System;
using System.Windows.Navigation;

namespace ToolDev_Lab03.Services
{
    public class SimpleNavigationService : INavigationService
    {
        private NavigationService navigationService;

        public SimpleNavigationService(NavigationService nativeService)
        {
            navigationService = nativeService;
        }

        public bool CanGoBack()
        {
            return navigationService.CanGoBack;
        }

        public void GoBack()
        {
            navigationService.GoBack();
        }

        public void GoForward()
        {
            navigationService.GoForward();
        }

        public bool CanGoForward()
        {
            return navigationService.CanGoForward;
        }

        public bool Navigate(string pageToken)
        {
            return navigationService.Navigate(new Uri(string.Format("Views/{0}Page.xaml", pageToken), UriKind.Relative));
        }
    }
}
