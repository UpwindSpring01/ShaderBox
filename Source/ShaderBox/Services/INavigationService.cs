using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ToolDev_Lab03.Services
{
    public interface INavigationService
    {
        bool CanGoBack();

        void GoBack();

        bool CanGoForward();

        void GoForward();
        
        bool Navigate(string pageToken);
    }
}
