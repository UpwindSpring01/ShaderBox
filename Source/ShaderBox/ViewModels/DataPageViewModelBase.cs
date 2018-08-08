using Microsoft.Win32;
using Prism.Commands;
using ShaderBox.Models;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;

namespace ShaderBox.ViewModels
{
    abstract class DataPageViewModelBase<T> where T : DataBase, new()
    {
        public ObservableCollection<T> Files { get; protected set; }

        private ObservableCollection<T> _selectedFiles = new ObservableCollection<T>();

        private bool _isRemoving = false;

        protected string _filters = null;
        protected int _filtersAllIndex = 0;

        public IList SelectedFiles
        {
            get
            {
                return _selectedFiles;
            }
            set
            {
                if (!_isRemoving)
                {
                    _selectedFiles.Clear();
                    foreach (T model in value)
                    {
                        _selectedFiles.Add(model);
                    }

                    RaiseRemoveFileCommand.RaiseCanExecuteChanged();
                }
            }
        }

        public DelegateCommand RaiseAddFileCommand { get; private set; }
        public DelegateCommand RaiseRemoveFileCommand { get; private set; }

        public DataPageViewModelBase()
        {
            RaiseAddFileCommand = new DelegateCommand(RaiseAddFile);
            RaiseRemoveFileCommand = new DelegateCommand(RaiseRemoveFile, () => CanExecuteRemoveFile());
        }

        private void RaiseAddFile()
        {
            OpenFileDialog fileDialog = new OpenFileDialog();
            fileDialog.Filter = _filters;
            fileDialog.Multiselect = true;
            fileDialog.FilterIndex = _filtersAllIndex;
            if (fileDialog.ShowDialog() == true)
            {
                foreach (string filePath in fileDialog.FileNames)
                {
                    T data = HandleAdd(filePath);

                    Files.Add(data);
                }
            }
        }

        protected abstract void HandleRemove(T data);

        protected abstract T HandleAdd(string filePath);

        private void RaiseRemoveFile()
        {
            _isRemoving = true;
            foreach (T file in _selectedFiles)
            {
                if (file.IsBuiltIn)
                {
                    continue;
                }

                HandleRemove(file);

                Files.Remove(file);
            }
            _isRemoving = false;
            _selectedFiles.Clear();
            RaiseRemoveFileCommand.RaiseCanExecuteChanged();
        }

        private bool CanExecuteRemoveFile()
        {
            return _selectedFiles.Any((m) => !m.IsBuiltIn);
        }
    }
}
