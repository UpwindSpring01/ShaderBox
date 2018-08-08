using Prism.Commands;
using Prism.Mvvm;
using ShaderBox.General;
using ShaderBox.Models;
using ShaderBox.Notifications;
using ShaderBox.UndoRedo;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace ShaderBox.ViewModels
{
    public class TopologyOption : BindableBase
    {
        public Topology Value { get; set; }
        public string Display { get; set; }

        private bool _isSelectable;

        public bool IsSelectable
        {
            get
            {
                return _isSelectable;
            }
            set
            {
                SetProperty(ref _isSelectable, value);
            }
        }
    }

    public class CullModeOption
    {
        public CullMode Value { get; set; }
        public string Display { get; set; }
    }

    public class FillModeOption
    {
        public FillMode Value { get; set; }
        public string Display { get; set; }
    }

    class ShaderWizardPageViewModel : UndoRedoBindableBaseRequestAware<ShaderGroupNotification>, IDataErrorInfo
    {
        private bool _isValid = false;
        private bool _isValidating = false;

        public IEnumerable<Tuple<ShaderGroupType, string>> ShaderTypes
        {
            get
            {
                yield return new Tuple<ShaderGroupType, string>(ShaderGroupType.Standard, "Standard");
                yield return new Tuple<ShaderGroupType, string>(ShaderGroupType.PostProcessing, "Post processing");
            }
        }

        public ObservableCollection<TopologyOption> TopologyTypes
        {
            get;
        } = new ObservableCollection<TopologyOption>()
        {
            new TopologyOption() { Value = Topology.TriangleList, Display = "Triangle List", IsSelectable = true },
            new TopologyOption() { Value = Topology.TriangleList_Adj, Display = "Triangle list with adjacency", IsSelectable = true },
            new TopologyOption() { Value = Topology.Patch_3Point, Display = "Patch list with 3 control points", IsSelectable = false },
        };

        public CullModeOption[] CullModeTypes
        {
            get;
        } = new CullModeOption[]
        {
            new CullModeOption() { Value = CullMode.Back, Display = "Back" },
            new CullModeOption() { Value = CullMode.Front, Display = "Front" },
            new CullModeOption() { Value = CullMode.None, Display = "None" },
        };

        public FillModeOption[] FillModeTypes
        {
            get;
        } = new FillModeOption[]
        {
            new FillModeOption() { Value = FillMode.Solid, Display = "Solid" },
            new FillModeOption() { Value = FillMode.Wireframe, Display = "Wireframe" },
        };

        public bool HasHullDomainShader
        {
            get
            {
                return _notification?.ShaderGroup.HasHullDomainShader ?? false;
            }
            set
            {
                if (value == true)
                {
                    SetValueMulti(_notification.ShaderGroup, new Tuple<object, string>[]
                        {
                        new Tuple<object, string>( value, "HasHullDomainShader" ),
                        new Tuple<object, string>( Topology.Patch_3Point, "Topology" )
                        });
                }
                else
                {
                    SetValueMulti(_notification.ShaderGroup, new Tuple<object, string>[]
                    {
                        new Tuple<object, string>( value, "HasHullDomainShader" ),
                        new Tuple<object, string>( Topology.TriangleList, "Topology" )
                    });
                }
            }
        }

        public bool HasGeometryShader
        {
            get
            {
                return _notification?.ShaderGroup.HasGeometryShader ?? false;
            }
            set
            {
                SetValue(_notification.ShaderGroup, value, "HasGeometryShader");
            }
        }

        public string Name
        {
            get
            {
                return _notification?.ShaderGroup.Name ?? "";
            }
            set
            {
                SetValue(_notification.ShaderGroup, value, "Name");
            }
        }

        public string Description
        {
            get
            {
                return _notification?.ShaderGroup.Description ?? "";
            }
            set
            {
                SetValue(_notification.ShaderGroup, value, "Description");
            }
        }

        public Topology Topology
        {
            get
            {
                return _notification?.ShaderGroup.Topology ?? Topology.TriangleList;
            }
            set
            {
                SetValue(_notification.ShaderGroup, value, "Topology");
            }
        }

        public CullMode CullMode
        {
            get
            {
                return _notification?.ShaderGroup.CullMode ?? CullMode.Back;
            }
            set
            {
                SetValue(_notification.ShaderGroup, value, "CullMode");
            }
        }

        public FillMode FillMode
        {
            get
            {
                return _notification?.ShaderGroup.FillMode ?? FillMode.Solid;
            }
            set
            {
                SetValue(_notification.ShaderGroup, value, "FillMode");
            }
        }

        private string _selectedTab = "/ShaderBox;component/Views/WizardGeneralTab.xaml";

        public string SelectedTabInternal
        {
            get
            {
                return _selectedTab;
            }
            set
            {
                _selectedTab = value;
                RaisePropertyChanged("SelectedTab");
            }
        }

        public string SelectedTab
        {
            get
            {
                return SelectedTabInternal;
            }
            set
            {
                SetValue(this, value, "SelectedTabInternal");
            }
        }

        private string _acceptButtonText;
        public string AcceptButtonText
        {
            get
            {
                return _acceptButtonText;
            }
            set
            {
                SetProperty(ref _acceptButtonText, value);
            }
        }

        public ICommand RaiseShaderTypeSelectionChangedCommand { get; private set; }
        public ICommand RaiseLoadedCommand { get; private set; }
        public ICommand RaiseUnloadedCommand { get; private set; }

        public ICommand RaiseNewShaderCommand { get; private set; }

        private bool _isInEditMode;
        private bool _isLoaded = false;

        private ShaderGroupType _originalGroupType;
        private bool _originalHasHullDomain;
        private bool _originalHasGeometry;

        public ShaderWizardPageViewModel()
        {
            RaiseShaderTypeSelectionChangedCommand = new DelegateCommand<SelectionChangedEventArgs>(RaiseShaderTypeSelectionChanged);
            RaiseNewShaderCommand = new DelegateCommand(RaiseNewShader);
            RaiseLoadedCommand = new DelegateCommand(RaiseLoaded);
            RaiseUnloadedCommand = new DelegateCommand(RaiseUnloaded);
        }

        private void RaiseShaderTypeSelectionChanged(SelectionChangedEventArgs e)
        {
            if (!_isLoaded || e.AddedItems.Count == 0)
            {
                return;
            }
            
            ShaderGroupType groupType = ((Tuple<ShaderGroupType, string>)e.AddedItems[0]).Item1;
            if (groupType == ShaderGroupType.PostProcessing)
            {
                SetValueMulti(_notification.ShaderGroup, new Tuple<object, string>[]
                    {
                        new Tuple<object, string>( false, "HasHullDomainShader" ),
                        new Tuple<object, string>( false, "HasGeometryShader" ),
                        new Tuple<object, string>( groupType, "ShaderGroupType" ),
            });
            }
            else
            {
                SetValueMulti(_notification.ShaderGroup, new Tuple<object, string>[]
                    {
                        new Tuple<object, string>( HasHullDomainShader, "HasHullDomainShader" ),
                        new Tuple<object, string>( HasGeometryShader, "HasGeometryShader" ),
                        new Tuple<object, string>( groupType, "ShaderGroupType" ),
                    });
            }
        }

        private void RaiseLoaded()
        {
            if (_notification.Content == null)
            {
                _isInEditMode = false;
                AcceptButtonText = "New";
            }
            else
            {
                _isInEditMode = true;
                _originalGroupType = _notification.ShaderGroup.ShaderGroupType;
                _originalHasHullDomain = _notification.ShaderGroup.HasHullDomainShader;
                _originalHasGeometry = _notification.ShaderGroup.HasGeometryShader;
                AcceptButtonText = "Edit";
            }
            
            _notification.ShaderGroup.PropertyChanged += new PropertyChangedEventHandler(OnResourcePropertyChanged);
            _notification.ShaderGroup.PropertyChanged += new PropertyChangedEventHandler(CheckTopologyOptions);
            UndoRedoSystem.CustomStack = new UndoRedoStack();

            // Properties are being set by ShaderEditorPageViewModel
            // So refresh them
            RaisePropertyChanged("Name");
            RaisePropertyChanged("Description");
            RaisePropertyChanged("Topology");
            RaisePropertyChanged("CullMode");
            RaisePropertyChanged("FillMode");
            RaisePropertyChanged("HasHullDomainShader");
            RaisePropertyChanged("HasGeometryShader");
            
            _isLoaded = true;
        }

        private void RaiseUnloaded()
        {
            _isLoaded = false;
            _notification.ShaderGroup.PropertyChanged -= new PropertyChangedEventHandler(OnResourcePropertyChanged);
            _notification.ShaderGroup.PropertyChanged -= new PropertyChangedEventHandler(CheckTopologyOptions);

            UndoRedoSystem.CustomStack = null;
        }

        private void CheckTopologyOptions(object sender, PropertyChangedEventArgs e)
        {
            if (_notification.ShaderGroup.ShaderGroupType == ShaderGroupType.PostProcessing)
            {
                TopologyTypes[0].IsSelectable = false;
                TopologyTypes[1].IsSelectable = false;
                TopologyTypes[2].IsSelectable = false;
            }
            else if (HasHullDomainShader)
            {
                TopologyTypes[0].IsSelectable = false;
                TopologyTypes[1].IsSelectable = false;
                TopologyTypes[2].IsSelectable = true;
            }
            else
            {
                TopologyTypes[0].IsSelectable = true;
                TopologyTypes[1].IsSelectable = true;
                TopologyTypes[2].IsSelectable = false;
            }
        }

        private void RaiseNewShader()
        {
            if (IsValid)
            {
                if (_isInEditMode)
                {
                    if ((_originalHasHullDomain && HasHullDomainShader == false)// Hull and domain will get deleted
                        || (_originalHasGeometry && HasGeometryShader == false))// Geometry will get deleted
                    {
                        MessageBoxResult result = MessageBox.Show("The new settings will result in permanently losing shaders\n\nDo you want to continue", "Warning", MessageBoxButton.YesNo, MessageBoxImage.Warning);
                        if (result != MessageBoxResult.Yes)
                        {
                            return;
                        }
                    }
                }
                ShaderGroup sg = _notification.ShaderGroup;
                Shader shader;
                if (sg.ShaderGroupType == ShaderGroupType.Standard)
                {
                    if (HasHullDomainShader)
                    {
                        shader = sg.Shaders.FirstOrDefault((s) => s.ShaderType == ShaderType.Hull);
                        if (shader == null)
                        {
                            sg.AddShader(new Shader("hs.hlsl", ShaderType.Hull));
                            sg.AddShader(new Shader("ds.hlsl", ShaderType.Domain));
                        }
                    }
                    else if (_isInEditMode)
                    {
                        shader = sg.Shaders.FirstOrDefault((s) => s.ShaderType == ShaderType.Hull);
                        shader?.Delete();
                        sg.Shaders.Remove(shader);

                        shader = sg.Shaders.FirstOrDefault((s) => s.ShaderType == ShaderType.Domain);
                        shader?.Delete();
                        sg.Shaders.Remove(shader);
                    }

                    if (HasGeometryShader)
                    {
                        shader = sg.Shaders.FirstOrDefault((s) => s.ShaderType == ShaderType.Geometry);
                        if (shader == null)
                        {
                            sg.AddShader(new Shader("gs.hlsl", ShaderType.Geometry));
                        }
                    }
                    else if (_isInEditMode)
                    {
                        shader = sg.Shaders.FirstOrDefault((s) => s.ShaderType == ShaderType.Geometry);
                        shader?.Delete();
                        sg.Shaders.Remove(shader);
                    }
                }
                else if (_isInEditMode)
                {
                    shader = sg.Shaders.FirstOrDefault((s) => s.ShaderType == ShaderType.Vertex);
                    shader?.Delete();
                    sg.Shaders.Remove(shader);
                    shader = sg.Shaders.FirstOrDefault((s) => s.ShaderType == ShaderType.Hull);
                    shader?.Delete();
                    sg.Shaders.Remove(shader);
                    shader = sg.Shaders.FirstOrDefault((s) => s.ShaderType == ShaderType.Domain);
                    shader?.Delete();
                    sg.Shaders.Remove(shader);
                    shader = sg.Shaders.FirstOrDefault((s) => s.ShaderType == ShaderType.Geometry);
                    shader?.Delete();
                    sg.Shaders.Remove(shader);
                }

                shader = sg.Shaders.FirstOrDefault((s) => s.ShaderType == ShaderType.Vertex);
                if (shader == null)
                {
                    sg.AddShader(new Shader("vs.hlsl", ShaderType.Vertex));
                }

                shader = sg.Shaders.FirstOrDefault((s) => s.ShaderType == ShaderType.Pixel);
                if (shader == null)
                {
                    sg.AddShader(new Shader("ps.hlsl", ShaderType.Pixel));
                }

                if (_isInEditMode)
                {
                    foreach (Shader s in sg.Shaders)
                    {
                        s.Group = sg;
                    }
                }
                _notification.Confirmed = true;
                FinishInteraction();
            }

        }

        public bool IsValid
        {
            get
            {
                _isValid = true;
                _isValidating = true;

                Error = "";
                RaisePropertyChanged("Name");
                RaisePropertyChanged("Description");

                _isValidating = false;

                return _isValid;
            }
        }

        private string _error = "";

        public string Error
        {
            get
            {
                return _error;
            }
            set
            {
                SetProperty(ref _error, value);
            }
        }

        public string this[string columnName]
        {
            get
            {
                // Prevent auto validating
                if (_isValidating)
                {
                    if (string.IsNullOrWhiteSpace(GetType().GetProperty(columnName).GetValue(this, null)?.ToString() ?? ""))
                    {
                        _isValid = false;
                        if (!string.IsNullOrEmpty(Error))
                        {
                            Error += "\n";
                        }
                        Error += "-" + columnName + " is a required field";
                        return "Required field";
                    }
                }
                return null;
            }
        }
    }
}

