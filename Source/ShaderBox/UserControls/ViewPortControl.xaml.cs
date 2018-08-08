using FirstFloor.ModernUI.Windows.Controls;
using ShaderBox.General;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;

namespace ShaderBox.UserControls
{
    /// <summary>
    /// Interaction logic for ViewportControl.xaml
    /// </summary>
    public partial class ViewportControl : UserControl
    {
        private TimeSpan _lastRender;
        private bool _lastVisible;

        private bool _forceSurfaceUpdate = false;

        private bool _hostLoaded = false;

        public string PageUri
        {
            get { return (string)GetValue(PageUriProperty); }
            set { SetValue(PageUriProperty, value); }
        }

        // Using a DependencyProperty as the backing store for PageUri.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty PageUriProperty =
            DependencyProperty.Register("PageUri", typeof(string), typeof(ViewportControl), new PropertyMetadata(""));



        public ViewportControl()
        {
            InitializeComponent();

            host.Loaded += Init;
            host.SizeChanged += Host_SizeChanged;
        }

        private void Host_Unloaded(object sender, RoutedEventArgs e)
        {
            if (_hostLoaded && ((ModernWindow)Application.Current.MainWindow).ContentSource.ToString() != PageUri)
            {
                _hostLoaded = false;
                _lastVisible = false;

                InteropImage.OnRender -= DoRender;
                CompositionTarget.Rendering -= CompositionTarget_Rendering;

                host.Loaded += Init;
            }
        }

        private void DoRender(IntPtr surface, bool isNewSurface)
        {
            if (((App)Application.Current).EngineInitialized)
            {
                if (_forceSurfaceUpdate)
                {
                    isNewSurface = true;
                    _forceSurfaceUpdate = false;
                }
                NativeMethods.InvokeWithDllProtection(() => NativeMethods.Render(surface, isNewSurface));
            }
        }

        private void Init(object sender, RoutedEventArgs e)
        {
            if (!_hostLoaded && ((ModernWindow)Application.Current.MainWindow).ContentSource.ToString() == PageUri)
            {
                _hostLoaded = true;

                host.Loaded -= Init;
                _forceSurfaceUpdate = true;
                InteropImage.WindowOwner = (new WindowInteropHelper(Application.Current.MainWindow)).Handle;
                InteropImage.OnRender = DoRender;

                host.Unloaded += Host_Unloaded;

                Host_SizeChanged(null, null);
                InteropImage.RequestRender();
            }
        }

        void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            RenderingEventArgs args = (RenderingEventArgs)e;

            // It's possible for Rendering to call back twice in the same frame 
            // so only render when we haven't already rendered in this frame.
            if (_lastRender != args.RenderingTime)
            {
                InteropImage.RequestRender();
                _lastRender = args.RenderingTime;
            }
        }

        private void Host_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            double dpiScale = 1.0; // default value for 96 dpi

            // determine DPI
            // (as of .NET 4.6.1, this returns the DPI of the primary monitor, if you have several different DPIs)
            var hwndTarget = PresentationSource.FromVisual(this).CompositionTarget as HwndTarget;
            if (hwndTarget != null)
            {
                dpiScale = hwndTarget.TransformToDevice.M11;
            }

            int surfWidth = (int)(host.ActualWidth < 0 ? 0 : Math.Ceiling(host.ActualWidth * dpiScale));
            int surfHeight = (int)(host.ActualHeight < 0 ? 0 : Math.Ceiling(host.ActualHeight * dpiScale));

            // Notify the D3D11Image of the pixel size desired for the DirectX rendering.
            // The D3DRendering component will determine the size of the new surface it is given, at that point.
            InteropImage.SetPixelSize(surfWidth, surfHeight);

            // Stop rendering if the D3DImage isn't visible - currently just if width or height is 0
            // TODO: more optimizations possible (scrolled off screen, etc...)
            bool isVisible = (surfWidth != 0 && surfHeight != 0);
            if (_lastVisible != isVisible)
            {
                _lastVisible = isVisible;
                if (_lastVisible)
                {
                    CompositionTarget.Rendering += CompositionTarget_Rendering;
                }
                else
                {
                    CompositionTarget.Rendering -= CompositionTarget_Rendering;
                }
            }
        }
    }
}
