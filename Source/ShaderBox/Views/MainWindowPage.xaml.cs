using FirstFloor.ModernUI.Windows.Controls;
using ShaderBox.Common;
using ShaderBox.General;
using System;
using System.Windows;

namespace ShaderBox.Views
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindowPage : ModernWindow
    {
        public MainWindowPage()
        {
            InitializeComponent();
            SourceInitialized += AttachChromeFixer;

            NativeMethods.InvokeWithDllProtection(() => NativeMethods.Init());
            ((App)Application.Current).EngineInitialized = true;
        }

         private void AttachChromeFixer(object sender, EventArgs e)
        {
            WindowChromeFixer.CompatibilityMaximizedNoneWindow(this);
            Initialized -= AttachChromeFixer;
        }


        private void ModernWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            ((App)Application.Current).EngineInitialized = false;
            NativeMethods.InvokeWithDllProtection(NativeMethods.Cleanup);
        }
    }
}