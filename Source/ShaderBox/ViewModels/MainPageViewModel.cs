using FirstFloor.ModernUI.Windows.Controls;
using Prism.Commands;
using Prism.Events;
using ShaderBox.Events;
using ShaderBox.General;
using ShaderBox.Models;
using ShaderBox.Models.Annotation;
using ShaderBoxBridge;
using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Data;
using System.Windows.Input;

namespace ShaderBox.ViewModels
{
    class MainPageViewModel : UndoRedoBindableBase, ITempHelper
    {
        public ListCollectionView ShadersViewSource
        {
            get;
            private set;
        }
        public Workspace Workspace { get; set; }

        public ICommand RaiseLoadedCommand { get; private set; }

        public ICommand RaiseEditShaderGroupCommand { get; private set; }
        public ICommand RaiseDeleteShaderGroupCommand { get; private set; }
        public ICommand RaiseSaveShaderPropertiesCommand { get; private set; }

        private ShaderGroup _selectedShaderGroup;
        public ShaderGroup SelectedShaderGroup
        {
            get
            {
                return _selectedShaderGroup;
            }
            set
            {
                _selectedShaderGroup = value;
                UpdateShaderViewport();
            }
        }

        public Model3DData SelectedModel
        {
            get
            {
                return Workspace.Models.FirstOrDefault((m) => m.Hash == (SelectedShaderGroup?.SelectedModel ?? 0));
            }
            set
            {
                if (SelectedShaderGroup != null)
                {
                    SetValue(SelectedShaderGroup, value?.Hash ?? Workspace.Models[0].Hash, () => ViewportHost.SetModel(SelectedModel?.Path ?? Workspace.Models[0].Path));
                }
            }
        }

        private ViewportHost _viewportHost;
        public ViewportHost ViewportHost
        {
            get
            {
                return _viewportHost;
            }
            set
            {
                _viewportHost = value;
                UpdateShaderViewport();
            }
        }

        private float _cameraOffset;
        public float CameraOffset
        {
            get { return _cameraOffset; }
            set
            {
                _cameraOffset = value;
                ViewportHost?.SetCameraOffset(value);
            }
        }

        public bool IsStandardShaderActive => SelectedShaderGroup.ShaderGroupType == ShaderGroupType.Standard;

        private ShaderStorageLinker _shaderStorageLinker;
        private EventAggregator _eventAggregator;

        public MainPageViewModel(ShaderStorageLinker shaderStorage, Workspace workspace, EventAggregator eventAggregator)
        {
            _shaderStorageLinker = shaderStorage;
            _eventAggregator = eventAggregator;

            Workspace = workspace;
            ShadersViewSource = new ListCollectionView(shaderStorage.ShaderGroups);
            ShadersViewSource.Filter = (s) =>
            {
                return ((ShaderGroup)s).ShaderGroupType != ShaderGroupType.SharedHeaders;
            };

            RaiseLoadedCommand = new DelegateCommand(RaiseLoaded);

            RaiseEditShaderGroupCommand = new DelegateCommand(RaiseEditShaderGroup);
            RaiseDeleteShaderGroupCommand = new DelegateCommand(RaiseDeleteShaderGroup);
            RaiseSaveShaderPropertiesCommand = new DelegateCommand(RaiseSaveShaderProperties);
        }

        private void UpdateShaderViewport()
        {
            if (ViewportHost != null)
            {
                if (_selectedShaderGroup != null && _selectedShaderGroup.IsBuilded)
                {
                    string[] shaderLocations = new string[5];
                    foreach (Shader s in _selectedShaderGroup.Shaders)
                    {
                        if (s.ShaderType != ShaderType.Header)
                        {
                            shaderLocations[(int)s.ShaderType] = $"{MainWindowPageViewModel.ShaderBoxResourcesFolderLocation}{_selectedShaderGroup.UniqueName}/.cso/{s.GetShaderTarget().Substring(0, 2)}.cso";
                        }
                    }
                    bool isStandard = IsStandardShaderActive;
                    if (isStandard)
                    {
                        ViewportHost.SetShaders(shaderLocations[0] ?? "", shaderLocations[1] ?? "", shaderLocations[2] ?? "", shaderLocations[3] ?? "", shaderLocations[4] ?? "");
                        ViewportHost.SetTopology((int)SelectedShaderGroup.Topology);
                        ViewportHost.SetRasterizerState((int)SelectedShaderGroup.CullMode, (int)SelectedShaderGroup.FillMode);
                    }
                    else
                    {
                        ViewportHost.SetPPShader(shaderLocations[4] ?? "");
                    }
                    foreach (AnnotationShaderGroup annotationShaderGroup in _selectedShaderGroup.AnnotationShaderGroups)
                    {
                        foreach (AnnotationGroup annotationGroup in annotationShaderGroup.Buffers)
                        {
                            foreach (AnnotationVariable annotationVariable in annotationGroup.AnnotationVariables)
                            {
                                annotationVariable.UpdateBuffer(false);
                            }
                            annotationGroup.MarshalBuffer(ViewportHost, isStandard);
                        }
                    }
                }

                ViewportHost.SetModel(SelectedModel?.Path ?? Workspace.Models[0].Path);
                ViewportHost.SetCameraOffset(CameraOffset);

                RaisePropertyChanged("SelectedModel");
            }
        }

        private void RaiseLoaded()
        {
            if (((ModernWindow)Application.Current.MainWindow).ContentSource.ToString() == "/ShaderBox;component/Views/MainPage.xaml")
            {
                ShadersViewSource.Refresh();
                ((App)Application.Current).ActiveViewport = this;
            }
        }

        private void RaiseEditShaderGroup()
        {
            if (_selectedShaderGroup != null)
            {
                _selectedShaderGroup.IsOpen = true;
                Workspace.Shaders.FirstOrDefault((s) => s.Path == _selectedShaderGroup.GetProjectPath()).IsOpen = true;

                Workspace.Save();

                _eventAggregator.GetEvent<RefreshExplorer>().Publish(null);
            }
        }

        private void RaiseDeleteShaderGroup()
        {
            if (_selectedShaderGroup != null)
            {
                MessageBoxResult result = Xceed.Wpf.Toolkit.MessageBox.Show($"Are you sure you want to delete the following shader group: {_selectedShaderGroup.Name}.", "Warning", MessageBoxButton.YesNo, MessageBoxImage.Warning);
                if (result == MessageBoxResult.Yes)
                {
                    _eventAggregator.GetEvent<RefreshExplorer>().Publish(_selectedShaderGroup);
                    Directory.Delete(Path.GetDirectoryName(Path.GetFullPath($"{ MainWindowPageViewModel.ShaderBoxResourcesFolderLocation}{_selectedShaderGroup.UniqueName}/")), true);
                    Workspace.Shaders.RemoveAll((s) => s.Path == _selectedShaderGroup.GetProjectPath());
                    _shaderStorageLinker.ShaderGroups.Remove(_selectedShaderGroup);

                    Workspace.Save();
                }
            }
        }

        private void RaiseSaveShaderProperties()
        {
            if (_selectedShaderGroup != null)
            {
                _selectedShaderGroup.Save(Workspace);
                ViewportHost.RenderThumbnail(Path.GetFullPath($"{MainWindowPageViewModel.ShaderBoxResourcesFolderLocation}{_selectedShaderGroup.UniqueName}/preview.png"),
                    async () =>
                    {
                        await Application.Current.Dispatcher.InvokeAsync(() =>
                        {
                            _selectedShaderGroup.Image = null;
                            ViewportHost.PopCallbackRenderThumbnail();
                        });
                    });
            }
        }
    }
}
