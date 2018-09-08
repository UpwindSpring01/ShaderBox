using Prism.Events;
using Prism.Mvvm;
using ShaderBox.ViewModels;
using ShaderBoxBridge;
using System.Windows;
using Unity;

namespace ShaderBox
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        public static readonly IUnityContainer Container = new UnityContainer();

        public bool EngineInitialized { get; set; } = false;

        // Temp workaround until, UI refactor
        internal ITempHelper ActiveViewport { get; set; }

        protected override void OnStartup(StartupEventArgs e)
        {
            EventAggregator eventAggregator = new EventAggregator();
            ViewModelLocationProvider.SetDefaultViewModelFactory((type) =>
            {
                return Container.Resolve(type);
            });

            Container.RegisterInstance(eventAggregator);
        }
    }
}
