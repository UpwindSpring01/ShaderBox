using ShaderBox.Common;
using ShaderBox.General;
using ShaderBox.Models;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.IO;

namespace ShaderBox.ViewModels
{
    class ImageDataPageViewModel : DataPageViewModelBase<ImageData>
    {
        public ImageDataPageViewModel(Workspace workspace)
        {
            Files = workspace.Images;

            ImageCodecInfo[] codecs = ImageCodecInfo.GetImageDecoders();
            _filters = "";
            string extensions = "";
            string sep = "";
            foreach (ImageCodecInfo codec in codecs)
            {
                string codecName = codec.CodecName.Substring(8).Replace("Codec", "Files").Trim();

                _filters += $"{sep}{codecName}|{codec.FilenameExtension}";
                sep = "|";
                extensions += $"{codec.FilenameExtension};";
                ++_filtersAllIndex;
            }

            IEnumerable<ImageDataDecoders.DecoderInfo> addCodecs = ImageDataDecoders.GetAdditionalDecoders();
            foreach (ImageDataDecoders.DecoderInfo codec in addCodecs)
            {

                _filters += $"{sep}{codec.FriendlyName}|{codec.FileExtensions}";
                extensions += $"{codec.FileExtensions};";
                ++_filtersAllIndex;
            }
            ++_filtersAllIndex;
            _filters += $"{sep}All Image Files (*.*)|{extensions.TrimEnd(';')}";
        }

        protected override ImageData HandleAdd(string filePath)
        {
            string name = Path.GetFileName(filePath);

            MainWindowPageViewModel.ShaderBoxImageFolderLocation.EnsureFolder();

            string imagePath = MainWindowPageViewModel.ShaderBoxImageFolderLocation +
                Path.GetFileNameWithoutExtension(name).CreateUniqueName(MainWindowPageViewModel.ShaderBoxImageFolderLocation, Path.GetExtension(filePath));

            File.Copy(filePath, imagePath);

            return new ImageData()
            {
                ImagePath = imagePath,
                Name = name
            };
        }

        protected override void HandleRemove(ImageData data)
        {
            if (File.Exists(data.ImagePath))
            {
                File.Delete(data.ImagePath);
            }
        }
    }
}
