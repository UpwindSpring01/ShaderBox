using ICSharpCode.AvalonEdit.Document;
using Prism.Mvvm;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Xml.Serialization;

namespace ShaderBox.Models
{
    public enum ShaderType
    {
        Vertex = 0,
        Hull = 1,
        Domain = 2,
        Geometry = 3,
        Pixel = 4,
        Header = 5,
    }

    public class Shader : BindableBase
    {
        public Shader()
        {
            IsModified = false;
        }

        public Shader(string name, ShaderType shaderType, string fileLocation = "")
        {
            Name = name;
            ShaderType = shaderType;
            FileLocation = fileLocation;
            IsModified = true;
        }

        [XmlElement("IsBuiltIn")]
        public bool IsBuiltIn
        {
            get;
            set;
        } = false;

        [XmlElement("ShaderType")]
        public ShaderType ShaderType { get; set; }


        private string _name;
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

        [XmlIgnore]
        public ShaderGroup Group { get; set; }

        [XmlElement("FileLocation")]
        public string FileLocation { get; set; }

        private TextDocument _document = null;

        [XmlIgnore]
        public TextDocument Document
        {
            get
            {
                if (_document == null)
                {
                    _document = new TextDocument();
                    // This gets called way to often
                    _document.UndoStack.PropertyChanged += (s, e) =>
                    {
                        IsModified = !_document.UndoStack.IsOriginalFile;
                    };
                    if (!string.IsNullOrWhiteSpace(FileLocation))
                    {
                        try
                        {
                            _document.Text = File.ReadAllText(FileLocation);
                            _document.UndoStack.ClearAll();
                            _document.UndoStack.MarkAsOriginalFile();
                        }
                        catch (Exception)
                        {
                        }
                    }
                    else
                    {
                        AddTemplateContent();
                    }
                }
                return _document;
            }
            set
            {
                _document = value;
            }
        }

        [XmlIgnore]
        public bool IsDeleteable
        {
            get
            {
                return ShaderType.Header == ShaderType && !IsBuiltIn;
            }
        }

        private bool _isModified;

        [XmlIgnore]
        public bool IsModified
        {
            get
            {
                return _isModified;
            }
            set
            {
                SetProperty(ref _isModified, value);
            }
        }
        [XmlIgnore]
        public ObservableCollection<string> Errors { get; set; } = new ObservableCollection<string>();

        private void AddTemplateContent()
        {
            switch (ShaderType)
            {
                case ShaderType.Vertex:
                    _document.Text = @"#include ""../.Shared/SB_Header.hlsli""

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}";
                    break;
                case ShaderType.Hull:
                    _document.Text = @"#include ""../.Shared/SB_Header.hlsli""

// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : WORLDPOS;
	// TODO: change/add other stuff
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : WORLDPOS; 
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	// Insert code to compute Output here
	Output.EdgeTessFactor[0] = 
		Output.EdgeTessFactor[1] = 
		Output.EdgeTessFactor[2] = 
		Output.InsideTessFactor = 15; // e.g. could calculate dynamic tessellation factors instead

	return Output;
}

[domain(""tri"")]
[partitioning(""fractional_odd"")]
[outputtopology(""triangle_cw"")]
[outputcontrolpoints(3)]
[patchconstantfunc(""CalcHSPatchConstants"")]
HS_CONTROL_POINT_OUTPUT main(
    InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
    uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
    HS_CONTROL_POINT_OUTPUT Output;

    // Insert code to compute Output here
    Output.vPosition = ip[i].vPosition;

    return Output;
}";
                    break;
                case ShaderType.Domain:
                    _document.Text = @"#include ""../.Shared/SB_Header.hlsli""

struct DS_OUTPUT
{
	float4 vPosition  : SV_POSITION;
	// TODO: change/add other stuff
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : WORLDPOS; 
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

[domain(""tri"")]
DS_OUTPUT main(
    HS_CONSTANT_DATA_OUTPUT input,
    float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
    DS_OUTPUT Output;

    Output.vPosition = float4(
        patch[0].vPosition * domain.x + patch[1].vPosition * domain.y + patch[2].vPosition * domain.z, 1);

    return Output;
}";
                    break;
                case ShaderType.Geometry:
                    _document.Text = @"#include ""../.Shared/SB_Header.hlsli""

struct GSOutput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(3)]
void main(
	triangle float4 input[3] : SV_POSITION, 
	inout TriangleStream< GSOutput > output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.pos = input[i];
		output.Append(element);
	}
}";
                    break;
                case ShaderType.Pixel:
                    _document.Text = @"#include ""../.Shared/SB_Header.hlsli""

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}";
                    break;
                case ShaderType.Header:
                    break;
                default:
                    break;
            }
        }

        public string GetShaderTarget()
        {
            switch (ShaderType)
            {
                case ShaderType.Vertex:
                    return "vs_5_0";
                case ShaderType.Hull:
                    return "hs_5_0";
                case ShaderType.Domain:
                    return "ds_5_0";
                case ShaderType.Geometry:
                    return "gs_5_0";
                case ShaderType.Pixel:
                    return "ps_5_0";
                case ShaderType.Header:
                default:
                    return "";
            }
        }
    }
}
