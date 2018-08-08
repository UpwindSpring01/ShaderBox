using FirstFloor.ModernUI.Windows.Controls;
using System.Windows;

namespace ShaderBox.Views
{
    /// <summary>
    /// Interaction logic for MainPage.xaml
    /// </summary>
    public partial class MainPage : ModernFrame
    {
        public MainPage()
        {
            InitializeComponent();
        }

        private void Expander_SetHandled(object sender, RoutedEventArgs e)
        {
            e.Handled = true;
        }
    }
}
