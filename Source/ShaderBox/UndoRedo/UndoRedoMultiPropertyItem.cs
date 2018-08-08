using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace ShaderBox.UndoRedo
{
    public class UndoRedoMultiPropertyItem : IUndoRedoItem
    {
        public List<UndoRedoPropertyItem> Properties = new List<UndoRedoPropertyItem>();

        public void DoCommand()
        {
            foreach(UndoRedoPropertyItem prop in Properties)
            {
                prop.DoCommand();
            }
        }

        public void Undo()
        {
            foreach (UndoRedoPropertyItem prop in Properties)
            {
                prop.Undo();
            }
        }

        public void Redo()
        {
            foreach (UndoRedoPropertyItem prop in Properties)
            {
                prop.Redo();
            }
        }
    }
}
