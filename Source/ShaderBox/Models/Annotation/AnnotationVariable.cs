using ShaderBox.General;
using ShaderBox.UserControls.Views;
using ShaderBox.ViewModels;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Xml.Serialization;

namespace ShaderBox.Models.Annotation
{
    public struct CustomKeyValuePair<K, V>
    {
        public K Key
        { get; set; }

        public V Value
        { get; set; }
    }

    public class AnnotationVariable : UndoRedoBindableBase
    {
        [XmlIgnore]
        public UIElement UIElement { get; set; }

        private string _name;

        [XmlAttribute("Name")]
        public string Name
        {
            get { return _name; }
            set { SetProperty(ref _name, value); }
        }

        [XmlAttribute("DataType")]
        public DataType DataType { get; set; }

        [XmlAttribute("IsSNorm")]
        public bool IsSNorm { get; set; }
        [XmlAttribute("IsUNorm")]
        public bool IsUNorm { get; set; }

        [XmlAttribute("Rows")]
        public uint Rows { get; set; }

        [XmlAttribute("Columns")]
        public uint Columns { get; set; }

        [XmlAttribute("IsScalar")]
        public bool IsScalar { get; set; }
        [XmlAttribute("IsVector")]
        public bool IsVector { get; set; }
        [XmlAttribute("IsMatrix")]
        public bool IsMatrix { get; set; }
        [XmlAttribute("IsArray")]
        public bool IsArray { get; set; }
        [XmlAttribute("IsTexture")]
        public bool IsTexture { get; set; }

        [XmlAttribute("Dimension")]
        public int Dimension { get; set; } = -1;

        [XmlAttribute("Offset")]
        public uint Offset { get; set; }
        [XmlAttribute("Size")]
        public uint Size { get; set; }

        [XmlElement("Value")]
        public AnnotationValue AnnotationValue { get; set; } = new AnnotationValue();

        [XmlElement("Settings")]
        public object Settings { get; set; }

        [XmlIgnore]
        public AnnotationGroup AnnotationGroup { get; set; }

        [XmlIgnore]
        public object Value
        {
            get { return AnnotationValue.Value; }
            set { SetValue(AnnotationValue, value, () => UpdateBuffer()); }
        }

        private Dictionary<string, string> _annotations = new Dictionary<string, string>();

        [XmlIgnore]
        public Dictionary<string, string> Annotations
        {
            get
            {
                if (_annotationProxy != null && _annotationProxy.Count > 0)
                {
                    _annotations = _annotationProxy.ToDictionary((v) => v.Key, (v) => v.Value);
                    _annotationProxy = null;
                }
                return _annotations;
            }
            set
            {
                _annotations = value;
            }
        }


        private List<CustomKeyValuePair<string, string>> _annotationProxy = new List<CustomKeyValuePair<string, string>>();

        [XmlArray("AnnotationKeywords")]
        [XmlArrayItem("KVPairSerializable")]
        public List<CustomKeyValuePair<string, string>> AnnotationsXMLHelper
        {
            get
            {
                _annotationProxy = Annotations?.Select((a) =>
                {
                    return new CustomKeyValuePair<string, string>() { Key = a.Key, Value = a.Value };
                })?.ToList() ?? new List<CustomKeyValuePair<string, string>>();

                return _annotationProxy;
            }
        }

        public void UpdateBuffer(bool marshalBuffer = true)
        {
            ITempHelper helper = ((App)Application.Current).ActiveViewport;
            if(helper == null || helper.ViewportHost == null)
            {
                return;
            }

            if (IsTexture)
            {
                if(helper.IsStandardShaderActive)
                {
                    helper.ViewportHost.UpdateShaderImage(uint.Parse(Regex.Match(AnnotationGroup.Register, @"\d+").Value), (int)AnnotationGroup.AnnotationShaderGroup.Type, Value?.ToString() ?? "");
                }
                else
                {
                    helper.ViewportHost.UpdatePPShaderImage(uint.Parse(Regex.Match(AnnotationGroup.Register, @"\d+").Value), Value?.ToString() ?? "");
                }
                return;
            }

            if (Value == null)
            {
                return;
            }
            if (IsScalar)
            {
                if (IsScalar)
                {
                    CopyToBuffer(PropertyHelpers.ConvertToCorrectType(DataType, Value));
                }
            }
            else if (IsVector)
            {
                if (Value is SerializableColor)
                {
                    CopyColorToBuffer((SerializableColor)Value, DataType.GetPackSizeScalar());
                }
            }
            if (marshalBuffer)
            {
                AnnotationGroup.MarshalBuffer(helper.ViewportHost, helper.IsStandardShaderActive);
            }
        }

        public void CopyColorToBuffer(SerializableColor color, uint stepOffset, uint startOffset = 0)
        {
            if (DataType.IsIntegral())
            {
                CopyToBuffer(PropertyHelpers.ConvertToCorrectType(DataType, color.R), startOffset);
                CopyToBuffer(PropertyHelpers.ConvertToCorrectType(DataType, color.G), startOffset + stepOffset);
                CopyToBuffer(PropertyHelpers.ConvertToCorrectType(DataType, color.B), startOffset + stepOffset * 2);
                if (Columns == 4)
                {
                    CopyToBuffer(PropertyHelpers.ConvertToCorrectType(DataType, color.A), startOffset + stepOffset * 3);
                }
            }
            else
            {
                CopyToBuffer(PropertyHelpers.ConvertToCorrectType(DataType, color.scR), startOffset);
                CopyToBuffer(PropertyHelpers.ConvertToCorrectType(DataType, color.scG), startOffset + stepOffset);
                CopyToBuffer(PropertyHelpers.ConvertToCorrectType(DataType, color.scB), startOffset + stepOffset * 2);
                if (Columns == 4)
                {
                    CopyToBuffer(PropertyHelpers.ConvertToCorrectType(DataType, color.scA), startOffset + stepOffset * 3);
                }
            }
        }

        public void CopyToBuffer(object correctedValue, uint subOffset = 0)
        {
            var size = Marshal.SizeOf(correctedValue);
            if (size != Size && IsScalar)
            {
                MessageBox.Show("Error in Packing logic");
            }

            IntPtr ptr = Marshal.AllocHGlobal(size);
            // Copy object byte-to-byte to unmanaged memory.
            Marshal.StructureToPtr(correctedValue, ptr, false);
            // Copy data from unmanaged memory to managed buffer.
            Marshal.Copy(ptr, AnnotationGroup.Bytes, (int)(Offset + subOffset), size);
            // Release unmanaged memory.
            Marshal.FreeHGlobal(ptr);
        }


        public void CreateUI()
        {
            AnnotationValue.PropertyChanged += new PropertyChangedEventHandler(OnResourcePropertyChanged);

            if (IsTexture)
            {
                UIElement = new ImagePicker(this);
            }
            else if (DataType.IsBool() && IsScalar)
            {
                UIElement = PropertyHelpers.CreateCheckBox(this, "Value", Value);
            }
            else if (!DataType.IsBool() && IsScalar && (IsSNorm || IsUNorm ||
                (Annotations?.Contains(new KeyValuePair<string, string>("UIWidget", "Slider")) ?? false)))
            {
                object val = Value;
                if (val == null)
                {
                    if (DataType.IsIntegral())
                    {
                        Value = 0;
                    }
                    else
                    {
                        Value = 0.0;
                    }
                }
                UIElement = PropertyHelpers.CreateSlider(this, "Value", this, val);
            }
            else if (!IsMatrix && (Columns == 3 || Columns == 4) &&
                (Annotations?.Contains(new KeyValuePair<string, string>("UIWidget", "Color")) ?? false))
            {
                UIElement = PropertyHelpers.CreateColorPicker(this, "Value", (Columns == 4) ? true : false, Value);
            }
            else if (IsScalar)
            {
                UIElement = PropertyHelpers.CreateSpinner(this, "Value", this, Value);
            }
            else
            {
                UIElement = new DropdownVecMatProperty(this);
            }
        }

        ~AnnotationVariable()
        {
            AnnotationValue.PropertyChanged -= new PropertyChangedEventHandler(OnResourcePropertyChanged);
        }
    }
}
