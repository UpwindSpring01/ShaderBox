using ShaderBox.General;
using ShaderBox.Models;
using ShaderBox.Models.Annotation;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Windows;
using System.Xml.Serialization;
using Unity;

namespace ShaderBox.ViewModels
{
    public class MainWindowPageViewModel : UndoRedoBindableBase
    {
        private const string ShaderBoxSharedFolderName = ".Shared";
        private const string ShaderBoxInternalFolderName = ".Internal";
        private const string ShaderBoxDataFolderName = ".Data";

        public const string ShaderBoxResourcesFolderLocation = "./Resources/";

        public const string ShaderBoxSharedFolderLocation = ShaderBoxResourcesFolderLocation + ShaderBoxSharedFolderName + "/";
        public const string ShaderBoxDataFolderLocation = ShaderBoxResourcesFolderLocation + ShaderBoxDataFolderName + "/";
        public const string ShaderBoxInternalFolderLocation = ShaderBoxResourcesFolderLocation + ShaderBoxInternalFolderName + "/";

        public const string ShaderBoxRootFileLocation = ShaderBoxInternalFolderLocation + "ShaderBox.xml";

        // Default shared project
        public const string ShaderBoxSharedProject = ShaderBoxSharedFolderLocation + "built-in.sbproj";
        private const string ShaderBoxBaseShaderLocation = ShaderBoxSharedFolderLocation + "SB_Header.hlsli";

        // Built-in data
        private const string ShaderBoxBuiltInModelsFolderLocation = ShaderBoxInternalFolderLocation + "Models/";
        private const string ShaderBoxBuiltInModel = ShaderBoxBuiltInModelsFolderLocation + "Cube.obj";
        private const string ShaderBoxBuiltInModelImage = ShaderBoxBuiltInModelsFolderLocation + "Cube.png";

        // User loaded data
        public const string ShaderBoxModelThumbnailFolderLocation = ShaderBoxDataFolderLocation + "Thumbnails/";
        public const string ShaderBoxImageFolderLocation = ShaderBoxDataFolderLocation + "Images/";
        public const string ShaderBoxModelFolderLocation = ShaderBoxDataFolderLocation + "Models/";

        private const string ShaderBoxBaseShaderContent =
@"// Same for every draw call in frame
cbuffer BaseConstantBufferFrame : register(b0)
{
	float4x4 gView : VIEW;
	float4x4 gProjection : PROJECTION;
	float4x4 gViewInverse : VIEWINVERSE;
	float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
};

cbuffer BaseConstantBufferObject : register(b1)
{
	float4x4 gWorld : WORLD;
	float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
	float4x4 gWorldInverse : WORLDINVERSE;
};

Texture2D gTexture : register(t0)

SamplerState samLinearWrap : register(s0);
SamplerState samLinearMirror : register(s1);
SamplerState samLinearClamp : register(s2);
SamplerState samPointWrap : register(s3);
SamplerState samPointMirror : register(s4);
SamplerState samPointClamp : register(s5);";

        private Workspace _workspace;

        public MainWindowPageViewModel()
        {
            Workspace workspace = null;

            try
            {
                if (File.Exists(ShaderBoxRootFileLocation))
                {
                    XmlSerializer serializer = new XmlSerializer(typeof(Workspace));
                    using (Stream stream = File.OpenRead(ShaderBoxRootFileLocation))
                    {
                        workspace = ((Workspace)serializer.Deserialize(stream));
                    }
                }
            }
            catch (Exception)
            {

            }
            finally
            {
                if (workspace == null)
                {
                    workspace = new Workspace();
                }
                if (workspace.Shaders.Count == 0 || workspace.Shaders.FirstOrDefault((s) => s.Path == ShaderBoxSharedProject) == null)
                {
                    workspace.Shaders.Insert(0, new ShaderPath()
                    {
                        Path = ShaderBoxSharedProject,
                        IsOpen = true
                    });
                
                }

                if(workspace.Models.Count == 0 || workspace.Models.FirstOrDefault((s) => s.Path == ShaderBoxBuiltInModel) == null)
                {
                    workspace.Models.Insert(0, new Model3DData()
                    {
                        Hash = 0,
                        ImagePath = ShaderBoxBuiltInModelImage,
                        IsBuiltIn = true,
                        Name = "Cube.obj",
                        Path = ShaderBoxBuiltInModel
                    });
                }

                if (!File.Exists(ShaderBoxSharedProject))
                {
                    ShaderGroup group = new ShaderGroup("Shared headers [../.Shared/]", ShaderGroupType.SharedHeaders)
                    {
                        UniqueName = ShaderBoxSharedFolderName,
                        IsOpen = true
                    };

                    Shader shader = new Shader("SB_Header.hlsli", ShaderType.Header, ShaderBoxBaseShaderLocation);
                    shader.IsBuiltIn = true;
                    group.AddShader(shader);
                    group.Save(workspace);
                }

                ShaderStorageLinker shaderStorage = new ShaderStorageLinker();
                XmlSerializer serializer = new XmlSerializer(typeof(ShaderGroup));
                List<ShaderPath> toDelete = new List<ShaderPath>();
                foreach (ShaderPath sPath in workspace.Shaders)
                {
                    try
                    {
                        using (Stream stream = File.OpenRead(sPath.Path))
                        {
                            ShaderGroup group = (ShaderGroup)serializer.Deserialize(stream);
                            group.IsOpen = sPath.IsOpen;
                            shaderStorage.ShaderGroups.Add(group);
                        }
                    }
                    catch (Exception e)
                    {
                        MessageBox.Show($"Failed to load project: {sPath.Path}\nError: {e}", "Load error", MessageBoxButton.OK, MessageBoxImage.Error);
                        toDelete.Add(sPath);
                    }
                }
                workspace.Shaders = workspace.Shaders.Except(toDelete).ToList();

                if (!File.Exists(ShaderBoxBaseShaderLocation))
                {
                    ShaderBoxSharedFolderLocation.EnsureFolder();
                    using (StreamWriter stream = new StreamWriter(ShaderBoxBaseShaderLocation))
                    {
                        stream.Write(ShaderBoxBaseShaderContent);
                    }
                }

                _workspace = workspace;
                _workspace.PropertyChanged += new PropertyChangedEventHandler(OnResourcePropertyChanged);
                App.Container.RegisterInstance(workspace);
                App.Container.RegisterInstance(shaderStorage);

                // Resolve cyclic dependency
                foreach (ShaderGroup shaderGroup in shaderStorage.ShaderGroups)
                {
                    foreach (Shader shader in shaderGroup.Shaders)
                    {
                        shader.Group = shaderGroup;
                    }
                    foreach(AnnotationShaderGroup annotationShaderGroup in shaderGroup.AnnotationShaderGroups)
                    {
                        annotationShaderGroup.CreateUI();
                    }
                    
                }
            }
        }

        public Uri SelectedPage
        {
            get
            {
                return new Uri(_workspace.SelectedPage, UriKind.Relative);
            }
            set
            {
                SetValue(_workspace, value.ToString());
            }
        }

        ~MainWindowPageViewModel()
        {
            _workspace.Save();
        }
    }
}