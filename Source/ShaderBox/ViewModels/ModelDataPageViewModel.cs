using ShaderBox.General;
using ShaderBox.Models;
using ShaderBoxBridge;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows;

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



            List<int> hashes = Files.Select((m) => m.Hash).ToList();
            int hash;
            do
            {
                hash = rand.Next(int.MaxValue);
            } while (hashes.Contains(hash));

            Model3DData data = new Model3DData()
            {
                Path = newLoc,
                Name = name,
                ImagePath = null,
                Hash = hash
            };

            Bridge.CreateThumbnailModel(filePath, imagePath,
                async () =>
                {
                    await Application.Current.Dispatcher.InvokeAsync(() =>
                    {
                        data.ImagePath = imagePath;
                        Bridge.PopCallbackRenderThumbnail();
                    });
                });

            return data;
        }
    }
}
