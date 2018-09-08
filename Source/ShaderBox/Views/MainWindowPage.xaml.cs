using FirstFloor.ModernUI.Windows.Controls;
using ShaderBox.Common;
using ShaderBox.General;
using System;
using System.ComponentModel;
using System.Windows;

namespace ShaderBox.Views
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindowPage : ModernWindow
    {
        public int WindowsToClose { get; set; }

        private int m_WindowsClosed = 0;

        private bool m_IsClosed = false;
        public int WindowsClosed
        {
            get { return m_WindowsClosed; }
            set
            {
                m_WindowsClosed = value;
                if (m_WindowsClosed == WindowsToClose)
                {
                    m_IsClosed = true;
                    ((App)Application.Current).EngineInitialized = false;
                    ShaderBoxBridge.Bridge.ShutdownEngine();

                    Application.Current.Shutdown();
                }
            }
        }

        public MainWindowPage()
        {
            InitializeComponent();
            SourceInitialized += AttachChromeFixer;

            ShaderBoxBridge.Bridge.InitEngine();
            ((App)Application.Current).EngineInitialized = true;
        }

        private void AttachChromeFixer(object sender, EventArgs e)
        {
            WindowChromeFixer.CompatibilityMaximizedNoneWindow(this);
            Initialized -= AttachChromeFixer;
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            if (!m_IsClosed)
            {
                base.OnClosing(e);
                e.Cancel = true;
            }
        }
    }
}