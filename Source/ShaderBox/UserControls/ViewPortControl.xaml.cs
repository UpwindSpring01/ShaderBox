using FirstFloor.ModernUI.Windows.Controls;
using ShaderBox.General;
using ShaderBox.Views;
using ShaderBoxBridge;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Interop;
using System.Windows.Media;

namespace ShaderBox.UserControls
{
    /// <summary>
    /// Interaction logic for ViewportControl.xaml
    /// </summary>
    public partial class ViewportControl : Border
    {
        public ViewportHost ViewportHost
        {
            get { return (ViewportHost)GetValue(ViewportHostProperty); }
            set { SetValue(ViewportHostProperty, value); }
        }

        public static readonly DependencyProperty ViewportHostProperty =
            DependencyProperty.Register("ViewportHost", typeof(ViewportHost),
                typeof(ViewportControl), new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.None));


        public ViewportControl()
        {
            InitializeComponent();
            Loaded += Border_Loaded;
            Application.Current.MainWindow.Closing += (s, e) =>
            {
                Child = null;
                ViewportHost.Dispose();
                ViewportHost = null;
            };
        }

        private async void ViewportWindowDestroyed()
        {
            await Application.Current.Dispatcher.InvokeAsync(() =>
            {
                ++((MainWindowPage)Application.Current.MainWindow).WindowsClosed;
            });
        }

        private async void ViewportWindowCreated()
        {
            await Application.Current.Dispatcher.InvokeAsync(() =>
            {
                Child = ViewportHost;
            });
        }

        private void Border_Loaded(object sender, RoutedEventArgs e)
        {
            Loaded -= Border_Loaded;

            Binding binding = new Binding();
            binding.Path = new PropertyPath("ViewportHost");
            binding.Source = DataContext;
            binding.Mode = BindingMode.OneWayToSource;
            BindingOperations.SetBinding(this, ViewportHostProperty, binding);

            ViewportHost = new ViewportHost(ViewportWindowCreated);
            ViewportHost.SetUnregisteredCallback(ViewportWindowDestroyed);
            ++((MainWindowPage)Application.Current.MainWindow).WindowsToClose;
        }
    }
}
