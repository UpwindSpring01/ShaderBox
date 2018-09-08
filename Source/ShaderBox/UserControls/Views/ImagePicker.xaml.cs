using ShaderBox.Models.Annotation;
using ShaderBox.UserControls.ViewModels;
using System.Windows.Controls;

namespace ShaderBox.UserControls.Views
{
    /// <summary>
    /// Interaction logic for ImagePicker.xaml
    /// </summary>
    public partial class ImagePicker : UserControl
    {
        public ImagePicker(AnnotationVariable annotationVar)
        {
            InitializeComponent();

            ((ImagePickerViewModel)DataContext).AnnotationVariable = annotationVar;
        }

        private void ComboBox_PreviewKeyUp(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if(e.Key == System.Windows.Input.Key.Delete)
            {
                cbx.SelectedItem = null;
                cbx.IsDropDownOpen = false;
            }
        }
    }
}
