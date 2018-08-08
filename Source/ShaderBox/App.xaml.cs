using Prism.Events;
using Prism.Mvvm;
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
