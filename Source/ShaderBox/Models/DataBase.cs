using System;
using System.IO;
using System.Windows.Media.Imaging;
using System.Xml.Serialization;

namespace ShaderBox.Models
{
    public class DataBase
    {
        [XmlAttribute("Name")]
        public string Name { get; set; }

        [XmlAttribute("IsBuiltIn")]
        public bool IsBuiltIn { get; set; }

        [XmlAttribute("ImagePath")]
        public string ImagePath { get; set; }

        private BitmapImage _image = null;

        [XmlIgnore]
        public BitmapImage Image
        {
            get
            {
                if (_image == null)
                {
                    if (!File.Exists(ImagePath))
                    {
                        return null;
                    }

                    // Create source
                    _image = new BitmapImage();

                    // BitmapImage.UriSource must be in a BeginInit/EndInit block
                    _image.BeginInit();
                    _image.CacheOption = BitmapCacheOption.OnLoad;
                    _image.UriSource = new Uri(ImagePath, UriKind.RelativeOrAbsolute);

                    // To save significant application memory, set the DecodePixelWidth or  
                    // DecodePixelHeight of the BitmapImage value of the image source to the desired 
                    // height or width of the rendered image. If you don't do this, the application will 
                    // cache the image as though it were rendered as its normal size rather then just 
                    // the size that is displayed.
                    // Note: In order to preserve aspect ratio, set DecodePixelWidth
                    // or DecodePixelHeight but not both.
                    _image.DecodePixelHeight = 200;
                    _image.EndInit();
                }
                return _image;
            }
        }
    }
}
