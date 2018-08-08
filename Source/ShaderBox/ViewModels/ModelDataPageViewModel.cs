using ShaderBox.General;
using ShaderBox.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace ShaderBox.ViewModels
{
    class ModelDataPageViewModel : DataPageViewModelBase<Model3DData>
    {
        private static Random rand = new Random();

        public ModelDataPageViewModel(Workspace workspace)
        {
            Files = workspace.Models;

            _filters = $"wavefront models (*.obj)|*.obj";
            _filtersAllIndex = 0;
        }

        protected override void HandleRemove(Model3DData data)
        {
            if (File.Exists(data.Path))
            {
                File.Delete(data.Path);
            }
            if (File.Exists(data.ImagePath))
            {
                File.Delete(data.ImagePath);
            }
        }

        protected override Model3DData HandleAdd(string filePath)
        {
            string name = Path.GetFileName(filePath);
            string newLoc = MainWindowPageViewModel.ShaderBoxModelFolderLocation +
                Path.GetFileNameWithoutExtension(name).CreateUniqueName(MainWindowPageViewModel.ShaderBoxModelFolderLocation, Path.GetExtension(filePath));

            MainWindowPageViewModel.ShaderBoxModelFolderLocation.EnsureFolder();
            File.Copy(filePath, newLoc, true);

            MainWindowPageViewModel.ShaderBoxModelThumbnailFolderLocation.EnsureFolder();

            string imagePath = MainWindowPageViewModel.ShaderBoxModelThumbnailFolderLocation +
                Path.GetFileNameWithoutExtension(name).CreateUniqueName(MainWindowPageViewModel.ShaderBoxModelThumbnailFolderLocation, ".png");

            NativeMethods.RenderThumbnail(filePath, imagePath);

            List<int> hashes = Files.Select((m) => m.Hash).ToList();
            int hash;
            do
            {
                hash = rand.Next(int.MaxValue);
            } while (hashes.Contains(hash));

            return new Model3DData()
            {
                Path = newLoc,
                Name = name,
                ImagePath = imagePath,
                Hash = hash
            };
        }
    }
}
