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
    }
}
