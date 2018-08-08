using Prism.Commands;
using ShaderBox.General;
using ShaderBox.Models;
using System.IO;
using System.Windows.Input;

namespace ShaderBox.ViewModels
{
    public partial class ShaderEditorPageViewModel : UndoRedoBindableBase
    {
        public ICommand RaiseSaveDocumentCommand { get; set; }
        public ICommand RaiseSaveShaderGroupCommand { get; set; }
        public ICommand RaiseSaveAllCommand { get; set; }
        public ICommand RaiseShaderNameChangedCommand { get; set; }


       // public ICommand RaiseSaveShaderGroupAsCommand { get; set; }

        public ICommand RaiseSaveShaderPropertiesCommand { get; private set; }

        private void InitializeSaveButtonCommands()
        {
            RaiseSaveDocumentCommand = new DelegateCommand<Shader>(RaiseSaveDocument);
            RaiseSaveShaderGroupCommand = new DelegateCommand<ShaderGroup>(RaiseSaveShaderGroup);
            RaiseSaveAllCommand = new DelegateCommand(RaiseSaveAll);

            RaiseSaveShaderPropertiesCommand = new DelegateCommand(RaiseSaveShaderProperties);
        }

        private void RaiseSaveAll()
        {
            foreach (Shader shader in ShaderGroupsView)
            {
                RaiseSaveDocument(shader);
            }
        }

        private void RaiseSaveDocument(Shader shader)
        {
            if (shader != null)
            {
                shader.Save(Workspace);
            }
        }

        //private void SaveAs(ShaderGroup group, string name)
        //{

        //}

        private void RaiseSaveShaderGroup(ShaderGroup group)
        {
            foreach (Shader shader in group.Shaders)
            {
                shader.Save(Workspace);
            }
        }

        private void RaiseSaveShaderProperties()
        {
            if(SelectedShaderGroup != null)
            {
                SelectedShaderGroup.Save(Workspace);
                NativeMethods.RenderThumbnailActive(Path.GetFullPath($"{MainWindowPageViewModel.ShaderBoxResourcesFolderLocation}{SelectedShaderGroup.UniqueName}/preview.png"));
                SelectedShaderGroup.Image = null;
            }
        }
    }
}
