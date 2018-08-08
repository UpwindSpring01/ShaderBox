using ShaderBox.Models.Annotation;
using ShaderBox.UserControls.ViewModels;
using System.Windows.Controls;

namespace ShaderBox.UserControls.Views
{
    /// <summary>
    /// Interaction logic for DropdownVecMatProperty.xaml
    /// </summary>
    public partial class DropdownVecMatProperty : UserControl
    {
        public DropdownVecMatProperty(AnnotationVariable annotationVar)
        {
            InitializeComponent();

            ((DropdownVecMatPropertyViewModel)DataContext).AnnotationVariable = annotationVar;
        }
    }
}
