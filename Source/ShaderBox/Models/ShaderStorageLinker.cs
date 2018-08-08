using System.Collections.ObjectModel;

namespace ShaderBox.Models
{
    public class ShaderStorageLinker
    {
        public ObservableCollection<ShaderGroup> ShaderGroups { get; set; } = new ObservableCollection<ShaderGroup>();
    }
}
