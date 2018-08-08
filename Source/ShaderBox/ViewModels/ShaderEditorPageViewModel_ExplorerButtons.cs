using Prism.Commands;
using ShaderBox.General;
using ShaderBox.Models;
using ShaderBox.Notifications;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Input;

namespace ShaderBox.ViewModels
{
    public partial class ShaderEditorPageViewModel : UndoRedoBindableBase
    {
        public ICommand RaiseAddHeaderCommand { get; set; }
        public ICommand RaiseRemoveHeaderCommand { get; set; }
        public ICommand RaiseMakeSharedHeaderCommand { get; set; }
        public ICommand RaiseCloseShaderGroupCommand { get; set; }
        public ICommand RaiseEditShaderGroupCommand { get; set; }

        private void InitializeExplorerButtons()
        {
            RaiseAddHeaderCommand = new DelegateCommand<ShaderGroup>(RaiseAddHeader);
            RaiseRemoveHeaderCommand = new DelegateCommand<Shader>(RaiseRemoveHeader);
            RaiseMakeSharedHeaderCommand = new DelegateCommand<Shader>(RaiseMakeSharedHeader);
            RaiseCloseShaderGroupCommand = new DelegateCommand<ShaderGroup>(RaiseCloseShaderGroup);
            RaiseEditShaderGroupCommand = new DelegateCommand<ShaderGroup>(RaiseEditShaderGroup);
        }

        private void RaiseEditShaderGroup(ShaderGroup group)
        {
            ShaderGroupNotification notification = new ShaderGroupNotification();
            notification.ShaderGroup = group.Copy();
            notification.Title = "Edit Shader";
            notification.Content = true;

            bool selectionNeedsUpdate = false;
            int selectedIndex = ShaderGroupsView.CurrentPosition;
            List<Shader> openItems = ShaderGroupsView.Cast<Shader>().ToList();
            int originalCount = group.Shaders.Count;
            int startIndex = 0;
            if (openItems[selectedIndex].Group == group)
            {
                selectionNeedsUpdate = true;
                startIndex = openItems.IndexOf(group.Shaders[0]);
            }

            ShaderGroupRequest.Raise(notification, (returned) =>
            {
                if (returned.Confirmed)
                {
                    int flattenedIndex = ShadersFlattened.IndexOf(group.Shaders[0]);

                    int index = _shaderStorage.ShaderGroups.IndexOf(group);
                    _shaderStorage.ShaderGroups.Remove(group);
                    _shaderStorage.ShaderGroups.Insert(index, notification.ShaderGroup);

                    group.AnnotationShaderGroups.Clear();
                    group.IsBuilded = false;

                    group.Shaders.ToList().ForEach((s) => ShadersFlattened.Remove(s));
                    for (int i = 0; i < notification.ShaderGroup.Shaders.Count; ++i)
                    {
                        ShadersFlattened.Insert(flattenedIndex + i, notification.ShaderGroup.Shaders[i]);
                    }

                    if (selectionNeedsUpdate)
                    {
                        selectedIndex = Math.Min(openItems.Count - 1, selectedIndex);
                        if (startIndex + notification.ShaderGroup.Shaders.Count <= selectedIndex)
                        {
                            selectedIndex = startIndex + notification.ShaderGroup.Shaders.Count - 1;
                        }
                        ShaderGroupsView.MoveCurrentToPosition(selectedIndex);
                    }
                }
            });
        }

        private void RaiseCloseShaderGroup(ShaderGroup group)
        {
            SetValue(group, false, () =>
            {
                group.Save(Workspace);
                Workspace.Shaders.FirstOrDefault((s) => s.Path == group.GetProjectPath()).IsOpen = group.IsOpen;
                ShaderGroupsView.Refresh();
            }, "IsOpen");
        }

        private void RaiseAddHeader(ShaderGroup group)
        {
            string name = "Header";
            IEnumerable<string> headers = group.Shaders.Where((s) => s.ShaderType == ShaderType.Header).Select((s) => s.Name);

            int counter = 0;
            while (headers.Contains(name + ".hlsli"))
            {
                ++counter;
                name = "Header_" + counter.ToString("D3");
            }

            Shader shader = new Shader(name + ".hlsli", ShaderType.Header);
            group.AddShader(shader);
            group.Save(Workspace);

            ShadersFlattened.Add(shader);
        }

        private void RaiseRemoveHeader(Shader shader)
        {
            if (MessageBox.Show("Are you sure you want to permanently delete this shader: \n\"" + shader.Name + "\"\nThis action is irreversible." +
                (shader.Group.ShaderGroupType == ShaderGroupType.SharedHeaders ? "\n\nThis is a SHARED shader and can result in breaking multiple groups" : ""),
                "Warning", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.Yes)
            {
                shader.Group.Shaders.Remove(shader);
                shader.Group.Save(Workspace);

                ShadersFlattened.Remove(shader);

                shader.Delete();
            }
        }

        private void RaiseMakeSharedHeader(Shader shader)
        {
            ShaderGroup originalGroup = shader.Group;

            shader.Group.Shaders.Remove(shader);

            string name = shader.Name.TrimEnd(".hlsli".ToArray());
            string newName = shader.Name;

            IEnumerable<string> headers = _shaderStorage.ShaderGroups[0].Shaders.Select((s) => s.Name);
            int counter = 0;
            while (headers.Contains(newName))
            {
                ++counter;
                newName = name + "_" + counter.ToString("D3") + ".hlsli";
            }

            shader.Name = newName;

            if (!string.IsNullOrEmpty(shader.FileLocation))
            {
                try
                {
                    File.Copy(shader.FileLocation, PATH_TO_SHARED_FOLDER + name);
                    File.Delete(shader.FileLocation);
                    shader.FileLocation = PATH_TO_SHARED_FOLDER + name;
                }
                catch (Exception) { }
            }

            _shaderStorage.ShaderGroups[0].AddShader(shader);

            originalGroup.Save(Workspace);
            _shaderStorage.ShaderGroups[0].Save(Workspace);

            ShaderGroupsView.Refresh();
        }
    }
}
