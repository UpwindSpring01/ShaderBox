using Prism.Mvvm;

namespace ShaderBox.Models
{
    public class ShaderEditorPageModel : BindableBase
    {
        private Shader _selectedShader;

        public Shader SelectedShader
        {
            get
            {
                return _selectedShader;
            }
            set
            {
                SetProperty(ref _selectedShader, value);
            }
        }
    }
}
