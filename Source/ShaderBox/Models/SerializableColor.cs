using System.Xml.Serialization;

namespace ShaderBox.Models
{
    public class SerializableColor
    {
        [XmlAttribute("r")]
        public byte R { get; set; }

        [XmlAttribute("g")]
        public byte G { get; set; }

        [XmlAttribute("b")]
        public byte B { get; set; }

        [XmlAttribute("a")]
        public byte A { get; set; }

        [XmlAttribute("scR")]
        public float scR { get; set; }

        [XmlAttribute("scG")]
        public float scG { get; set; }

        [XmlAttribute("scB")]
        public float scB { get; set; }

        [XmlAttribute("scA")]
        public float scA { get; set; }
    }
}
