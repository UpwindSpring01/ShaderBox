using Prism.Mvvm;
using ShaderBox.Models.Annotation;
using ShaderBox.ViewModels;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Runtime.CompilerServices;
using System.Windows.Media.Imaging;
using System.Xml.Serialization;

namespace ShaderBox.Models
{
    public enum ShaderGroupType
    {
        Standard,
        PostProcessing,
        SharedHeaders
    }

    public enum Topology
    {
        TriangleList = 0,
        TriangleList_Adj = 1,
        Patch_3Point = 2
    }

    public enum CullMode
    {
        Back = 0,
        Front = 1,
        None = 2
    }

    public enum FillMode
    {
        Solid = 0,
        Wireframe = 1
    }

    [XmlRoot("ShaderGroup")]
    public class ShaderGroup : INotifyPropertyChanged
    {
        public ShaderGroup()
        {
        }

        public ShaderGroup(string name, ShaderGroupType shaderGroupType)
        {
            Name = name;
            ShaderGroupType = shaderGroupType;
        }

        [XmlElement("NameIdentifier")]
        public string UniqueName { get; set; } = "";

        [XmlIgnore]
        public bool HasUniqueName { get { return !string.IsNullOrEmpty(UniqueName); } }

        private string _name = "";

        [XmlElement("Name")]
        public string Name
        {
            get
            {
                return _name;
            }
            set
            {
                SetProperty(ref _name, value);
            }
        }

        private string _description = "";

        [XmlElement("Description")]
        public string Description
        {
            get
            {
                return _description;
            }
            set
            {
                SetProperty(ref _description, value);
            }
        }

        [XmlElement("IsBuilded")]
        public bool IsBuilded
        {
            get;
            set;
        }

        [XmlArray("AnnotationShaders"), XmlArrayItem(typeof(AnnotationShaderGroup), ElementName = "Annotations")]
        public ObservableCollection<AnnotationShaderGroup> AnnotationShaderGroups
        {
            get;
            set;
        } = new ObservableCollection<AnnotationShaderGroup>();

        private ShaderGroupType _shaderGroupType = ShaderGroupType.Standard;

        [XmlElement("ShaderGroupType")]
        public ShaderGroupType ShaderGroupType
        {
            get
            {
                return _shaderGroupType;
            }
            set
            {
                SetProperty(ref _shaderGroupType, value);
            }
        }

        private Topology _topology = Topology.TriangleList;

        [XmlElement("Topology")]
        public Topology Topology
        {
            get
            {
                return _topology;
            }
            set
            {
                SetProperty(ref _topology, value);
            }
        }

        private CullMode _cullMode = CullMode.Back;

        [XmlElement("CullMode")]
        public CullMode CullMode
        {
            get
            {
                return _cullMode;
            }
            set
            {
                SetProperty(ref _cullMode, value);
            }
        }

        private FillMode _fillMode = FillMode.Solid;

        [XmlElement("FillMode")]
        public FillMode FillMode
        {
            get
            {
                return _fillMode;
            }
            set
            {
                SetProperty(ref _fillMode, value);
            }
        }

        private bool _hasHullDomainShader = false;

        [XmlElement("HasHullDomainShader")]
        public bool HasHullDomainShader
        {
            get
            {
                return _hasHullDomainShader;
            }
            set
            {
                SetProperty(ref _hasHullDomainShader, value);
            }
        }

        private bool _hasGeometryShader = false;

        [XmlElement("HasGeometryShader")]
        public bool HasGeometryShader
        {
            get
            {
                return _hasGeometryShader;
            }
            set
            {
                SetProperty(ref _hasGeometryShader, value);
            }
        }

        private int _selectedModel;

        [XmlElement("SelectedModel")]
        public int SelectedModel
        {
            get
            {
                return _selectedModel;
            }
            set
            {
                SetProperty(ref _selectedModel, value);
            }
        }

        [XmlElement("Shaders")]
        public ObservableCollection<Shader> Shaders { get; set; } = new ObservableCollection<Shader>();


        private bool _isOpen = true;
        [XmlIgnore]
        public bool IsOpen
        {
            get
            {
                return _isOpen;
            }
            set
            {
                SetProperty(ref _isOpen, value);
            }
        }

        public void AddShader(Shader shader)
        {
            Shaders.Add(shader);
            shader.Group = this;
        }

        private BitmapImage _image = null;

        [XmlIgnore]
        public BitmapImage Image
        {
            get
            {
                if (_image == null)
                {
                    string path = Path.GetFullPath($"{MainWindowPageViewModel.ShaderBoxResourcesFolderLocation}{UniqueName}/preview.png");
                    if (!File.Exists(path))
                    {
                        return null;
                    }

                    // Create source
                    _image = new BitmapImage();

                    // BitmapImage.UriSource must be in a BeginInit/EndInit block
                    _image.BeginInit();
                    _image.CacheOption = BitmapCacheOption.OnLoad;
                    _image.CreateOptions = BitmapCreateOptions.IgnoreImageCache;
                    _image.UriSource = new Uri(path, UriKind.Absolute);

                    // To save significant application memory, set the DecodePixelWidth or  
                    // DecodePixelHeight of the BitmapImage value of the image source to the desired 
                    // height or width of the rendered image. If you don't do this, the application will 
                    // cache the image as though it were rendered as its normal size rather then just 
                    // the size that is displayed.
                    // Note: In order to preserve aspect ratio, set DecodePixelWidth
                    // or DecodePixelHeight but not both.
                    _image.DecodePixelHeight = 200;
                    _image.EndInit();
                }
                return _image;
            }
            set
            {
                SetProperty(ref _image, value);
            }
        }

        [field:XmlIgnore]
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual bool SetProperty<T>(ref T storage, T value, [CallerMemberName] string propertyName = null)
        {
            if (Equals(storage, value))
            {
                return false;
            }

            storage = value;
            OnPropertyChanged(propertyName);

            return true;
        }

        protected void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
