using Prism.Mvvm;
using System.Collections.ObjectModel;
using System.Xml.Serialization;

namespace ShaderBox.Models.Annotation
{
    public class AnnotationValue : BindableBase
    {
        private object _value;

        [XmlElement("Value_d", typeof(double))]
        [XmlElement("Value_ui", typeof(uint))]
        [XmlElement("Value_i", typeof(int))]
        [XmlElement("Value_f", typeof(float))]
        [XmlElement("Value_b", typeof(bool))]
        [XmlElement("Color", typeof(SerializableColor))]
        [XmlElement("ColorArray", typeof(ObservableCollection<SerializableColor>))]
        [XmlElement("ValueArray", typeof(ObservableCollection<object>))]
        [XmlElement("Path", typeof(string))]
        public object Value
        {
            get { return _value; }
            set { SetProperty(ref _value, value); }
        }
    }
}
