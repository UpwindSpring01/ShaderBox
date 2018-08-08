using System;
using System.IO;
using System.Windows.Media.Imaging;
using System.Xml.Serialization;

namespace ShaderBox.Models
{
    public class Model3DData : DataBase
    {
        [XmlAttribute("Path")]
        public string Path { get; set; }

        [XmlAttribute("Hash")]
        public int Hash { get; set; }
    }
}
