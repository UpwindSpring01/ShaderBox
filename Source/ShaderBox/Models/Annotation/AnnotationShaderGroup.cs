using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Xml.Serialization;

namespace ShaderBox.Models.Annotation
{
    public class AnnotationShaderGroup
    {
        [XmlElement("ShaderType")]
        public ShaderType Type { get; set; }

        [XmlArray("Buffers"), XmlArrayItem(typeof(AnnotationGroup), ElementName = "Buffer")]
        public ObservableCollection<AnnotationGroup> Buffers { get; set; } = new ObservableCollection<AnnotationGroup>();

        // Buffers combined
        [XmlIgnore]
        public ObservableCollection<AnnotationVariable> Elements { get; set; }

        public void CreateUI()
        {
            Elements = new ObservableCollection<AnnotationVariable>();
            foreach (AnnotationGroup group in Buffers)
            {
                group.AnnotationShaderGroup = this;
                group.Bytes = new byte[group.BufferSize];
                foreach (AnnotationVariable var in group.AnnotationVariables)
                {
                    var.AnnotationGroup = group;
                    var.CreateUI();
                    Elements.Add(var);
                }
            }
        }

    }
}
