using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ShaderBox.UndoRedo
{
    public interface IUndoRedoItem
    {
        void DoCommand();
        void Undo();
        void Redo();

    }
}
