using ShaderBox.General;
using ShaderBoxBridge;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Xml.Serialization;

namespace ShaderBox.Models.Annotation
{
    public class AnnotationGroup
    {
        [XmlElement("Register")]
        public string Register { get; set; }

        [XmlElement("BufferSize")]
        public uint BufferSize { get; set; }

        [XmlIgnore]
        public byte[] Bytes { get; set; }

        [XmlArray("Variables"), XmlArrayItem(typeof(AnnotationVariable), ElementName = "Variable")]
        public List<AnnotationVariable> AnnotationVariables { get; set; } = new List<AnnotationVariable>();

        [XmlIgnore]
        public AnnotationShaderGroup AnnotationShaderGroup { get; set; }

        public void MarshalBuffer(ViewportHost host, bool isStandard)
        {
            if (Bytes.Length != 0)
            {
                if (isStandard)
                {
                    host.UpdateShaderVariables(uint.Parse(Regex.Match(Register, @"\d+").Value), (int)AnnotationShaderGroup.Type,
                       Bytes, (uint)Bytes.Length);
                }
                else
                {
                    host.UpdatePPShaderVariables(uint.Parse(Regex.Match(Register, @"\d+").Value),
                        Bytes, (uint)Bytes.Length);
                }

            }
        }
    }
}
