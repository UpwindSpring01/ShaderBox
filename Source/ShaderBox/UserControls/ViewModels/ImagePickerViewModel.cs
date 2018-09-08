using Prism.Mvvm;
using ShaderBox.Models;
using ShaderBox.Models.Annotation;
using System.Collections.ObjectModel;
using System.Linq;

namespace ShaderBox.UserControls.ViewModels
{
    public class ImagePickerViewModel : BindableBase
    {
        private AnnotationVariable _annotionVariable;
        public AnnotationVariable AnnotationVariable
        {
            get
            {
                return _annotionVariable;
            }
            set
            {
                SetProperty(ref _annotionVariable, value);
                _annotionVariable.PropertyChanged += (s, e) =>
                {
                    if (e.PropertyName == "Value")
                    {
                        if ((_selectedImage?.ImagePath ?? "") != (AnnotationVariable.Value?.ToString() ?? ""))
                        {
                            _selectedImage = Images.FirstOrDefault((i) => i.ImagePath == (AnnotationVariable.Value?.ToString() ?? ""));
                            RaisePropertyChanged("SelectedImage");
                        }
                    }
                };
                if (_annotionVariable.AnnotationValue.Value != null)
                {
                    _selectedImage = Images.FirstOrDefault((i) => i.ImagePath == (AnnotationVariable.Value?.ToString() ?? ""));
                    RaisePropertyChanged("SelectedImage");
                }
                else if (_annotionVariable.Annotations?.ContainsKey("ResourceName") ?? false)
                {
                    // Not using _annotionVariable.Value because this would cause an undo to be registered
                    _annotionVariable.AnnotationValue.Value = Images.FirstOrDefault((i => i.Name == _annotionVariable.Annotations["ResourceName"]))?.ImagePath ?? "";
                }
                _annotionVariable.UpdateBuffer();
            }
        }

        private ImageData _selectedImage;

        public ImageData SelectedImage
        {
            get
            {
                return _selectedImage;
            }
            set
            {
                SetProperty(ref _selectedImage, value);
                AnnotationVariable.Value = value?.ImagePath ?? "";
            }
        }

        public ObservableCollection<ImageData> Images { get; private set; }

        public ImagePickerViewModel(Workspace workspace)
        {
            Images = workspace.Images;

            // In case of undo and setting from code behind

        }
    }
}
