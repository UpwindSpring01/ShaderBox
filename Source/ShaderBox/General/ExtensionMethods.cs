using ShaderBox.Models;
using ShaderBox.ViewModels;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Media.Imaging;
using System.Xml.Serialization;

namespace ShaderBox.General
{
    public static class ExtensionMethods
    {
        public static ShaderGroup Copy(this ShaderGroup group)
        {
            return new ShaderGroup()
            {
                Name = group.Name,
                ShaderGroupType = group.ShaderGroupType,
                Topology = group.Topology,
                Description = group.Description,
                UniqueName = group.UniqueName,
                IsOpen = group.IsOpen,
                HasHullDomainShader = group.HasHullDomainShader,
                HasGeometryShader = group.HasGeometryShader,
                CullMode = group.CullMode,
                FillMode = group.FillMode,

                Shaders = new ObservableCollection<Shader>(group.Shaders)
            };
        }

        public static void Delete(this Shader shader)
        {
            if (!string.IsNullOrEmpty(shader.FileLocation))
            {
                try
                {
                    File.Delete(shader.FileLocation);
                }
                catch (Exception) { }
            }
        }

        public static void Save(this Shader shader, Workspace workspace)
        {
            if (!shader.IsModified && !string.IsNullOrEmpty(shader.FileLocation))
            {
                return;
            }
            try
            {
                // Enforce to generate templated content
                string text = shader.Document.Text;

                if (string.IsNullOrEmpty(shader.FileLocation))
                {
                    shader.Group.CreateUniqueName();
                    shader.FileLocation = $"{MainWindowPageViewModel.ShaderBoxResourcesFolderLocation}{shader.Group.UniqueName}/{shader.Name}";

                    shader.Group.Save(workspace);
                }
                using (StreamWriter stream = new StreamWriter(shader.FileLocation))
                {
                    stream.Write(text);
                }
                shader.Document.UndoStack.MarkAsOriginalFile();
            }
            catch (Exception e)
            {
                MessageBox.Show($"Saving failed: {shader.Name}\n{e}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        public static void Save(this ShaderGroup shadergroup, Workspace workspace)
        {
            shadergroup.CreateUniqueName();

            string path = shadergroup.GetProjectPath();

            if (workspace.Shaders.FirstOrDefault((s) => s.Path == path) == null)
            {
                workspace.Shaders.Add(new ShaderPath()
                {
                    Path = path,
                    IsOpen = true
                });
            }
            path.EnsureFolder();

            XmlSerializer serializer = new XmlSerializer(typeof(ShaderGroup));
            using (Stream stream = File.Create(path))
            {
                serializer.Serialize(stream, shadergroup);
            }
        }

        public static void Save(this Workspace workspace)
        {
            XmlSerializer serializer = new XmlSerializer(typeof(Workspace));
            using (Stream stream = File.Create(MainWindowPageViewModel.ShaderBoxRootFileLocation))
            {
                serializer.Serialize(stream, workspace);
            }
        }

        public static string GetProjectPath(this ShaderGroup shadergroup)
        {
            string path;
            if (shadergroup.ShaderGroupType == ShaderGroupType.SharedHeaders)
            {
                path = MainWindowPageViewModel.ShaderBoxSharedProject;
            }
            else
            {
                path = $"{MainWindowPageViewModel.ShaderBoxResourcesFolderLocation}{shadergroup.UniqueName}/{shadergroup.Name}.sbproj";
            }
            return path;
        }

        private static void CreateUniqueName(this ShaderGroup group)
        {
            if (!group.HasUniqueName)
            {
                group.UniqueName = CreateUniqueName(group.Name, MainWindowPageViewModel.ShaderBoxResourcesFolderLocation, "");
            }
        }

        public static string CreateUniqueName(this string filename, string folderLocation, string extension)
        {
            string baseName = filename + "_" + DateTime.Now.ToString("yyyy_M_d-HH_mm");
            string name = baseName;
            if (Directory.Exists(folderLocation))
            {
                IEnumerable<string> headers = Directory.EnumerateFileSystemEntries(folderLocation);
                int counter = 0;
                while (headers.Contains(name + extension))
                {
                    ++counter;
                    name = baseName + counter.ToString("D3");
                }
            }
            return name + extension;
        }

        public static void Move(this Shader shader, Workspace workspace)
        {
            if (string.IsNullOrEmpty(shader.FileLocation))
            {
                return;
            }
            try
            {
                string oldFileLocation = shader.FileLocation;
                shader.FileLocation = $"{MainWindowPageViewModel.ShaderBoxResourcesFolderLocation}{shader.Group.UniqueName}/{shader.Name}";
                File.Move(oldFileLocation, shader.FileLocation);

                shader.Group.Save(workspace);

            }
            catch (Exception e)
            {
                MessageBox.Show($"Renaming shader on disk failed: {shader.Name}\n{e}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        public static uint RoundUpMultiple16(this uint numToRound)
        {
            return (numToRound + 15u) & ~(15u); // Only works when multiple is a power of 2
        }

        public static void EnsureFolder(this string path)
        {
            string directoryName = Path.GetDirectoryName(path);
            if ((directoryName.Length > 0) && (!Directory.Exists(directoryName)))
            {
                Directory.CreateDirectory(directoryName);
            }
        }
    }
}
