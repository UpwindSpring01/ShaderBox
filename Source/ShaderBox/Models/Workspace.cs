using Prism.Mvvm;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Xml.Serialization;

namespace ShaderBox.Models
{
    public class Workspace : BindableBase
    {
        private int _selectedIndex;

        [XmlElement("SelectedIndex")]
        public int SelectedIndex
        {
            get
            {
                return _selectedIndex;
            }
            set
            {
                SetProperty(ref _selectedIndex, value);
            }
        }

        private string _selectedPage = "/ShaderBox;component/Views/MainPage.xaml";

        [XmlElement("SelectedPage")]
        public string SelectedPage
        {
            get
            {
                return _selectedPage;
            }
            set
            {
                SetProperty(ref _selectedPage, value);
            }
        }

        [XmlArray("Shadergroups"), XmlArrayItem(typeof(ShaderPath), ElementName = "ShaderGroup")]
        public List<ShaderPath> Shaders { get; set; } = new List<ShaderPath>();

        [XmlArray("Images"), XmlArrayItem(typeof(ImageData), ElementName = "Image")]
        public ObservableCollection<ImageData> Images { get; set; } = new ObservableCollection<ImageData>();

        [XmlArray("Models"), XmlArrayItem(typeof(Model3DData), ElementName = "Model")]
        public ObservableCollection<Model3DData> Models { get; set; } = new ObservableCollection<Model3DData>();
    }

    public class ShaderPath : BindableBase
    {
        [XmlAttribute("Path")]
        public string Path { get; set; }

        private bool _isOpen = true;

        [XmlAttribute("IsOpen")]
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
    }
}
