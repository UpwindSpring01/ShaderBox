using Prism.Commands;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace ShaderBox.UndoRedo
{
    public class UndoRedoPropertyItem : IUndoRedoItem
    {
        private PropertyInfo _propInfo = null;

        public UndoRedoPropertyItem()
        {
        }

        public void DoCommand()
        {
            OldValue = PropInfo.GetValue(ModelObject, null);
            Redo();
        }

        public void Undo()
        {
            PropInfo.SetValue(ModelObject, OldValue, null);
            Command?.Invoke();
        }

        public void Redo()
        {
            PropInfo.SetValue(ModelObject, NewValue, null);
            Command?.Invoke();
        }

        private object OldValue { get; set; }

        public object NewValue { get; set; }

        public string PropertyStr { get; set; }

        public object ModelObject { get; set; }

        public Action Command { get; set; }

        private PropertyInfo PropInfo
        {
            get
            {
                if (null == _propInfo)
                {
                    Type type = ModelObject.GetType();
                    _propInfo = type.GetProperty(PropertyStr);
                }
                return _propInfo;
            }
        }
    }
}
