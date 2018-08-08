using Prism.Commands;
using Prism.Mvvm;
using ShaderBox.UndoRedo;
using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Input;
using System.Xml.Serialization;

namespace ShaderBox.General
{
    public class UndoRedoBindableBase : BindableBase
    {
        [XmlIgnore]
        public ICommand RaiseKeyDownCommand { get; private set; }

        public UndoRedoBindableBase()
        {
            RaiseKeyDownCommand = new DelegateCommand<KeyEventArgs>(RaiseKeyDown);
        }

        private void RaiseKeyDown(KeyEventArgs e)
        {

            if ((Keyboard.Modifiers & ModifierKeys.Control) == ModifierKeys.Control)
            {
                if (e.Key == Key.Z)
                {
                    UndoRedoSystem.Undo();
                }
                else if (e.Key == Key.Y)
                {
                    UndoRedoSystem.Redo();
                }
            }
        }

        protected void SetValue(object model, object value, Action command = null, [CallerMemberName] string propName = null)
        {
            UndoRedoPropertyItem propItem = new UndoRedoPropertyItem();
            propItem.ModelObject = model;
            propItem.PropertyStr = propName;
            propItem.NewValue = value;
            propItem.Command = command;
            propItem.DoCommand();
            UndoRedoSystem.AddToUndoStack(propItem);
        }

        protected void SetValueMulti(object model, params Tuple<object, string>[] props)
        {
            UndoRedoMultiPropertyItem multiPropItem = new UndoRedoMultiPropertyItem();
            foreach (Tuple<object, string> prop in props)
            {
                UndoRedoPropertyItem propItem = new UndoRedoPropertyItem();
                propItem.ModelObject = model;
                propItem.PropertyStr = prop.Item2;
                propItem.NewValue = prop.Item1;

                multiPropItem.Properties.Add(propItem);
            }
            multiPropItem.DoCommand();
            UndoRedoSystem.AddToUndoStack(multiPropItem);
        }

        protected void OnResourcePropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            RaisePropertyChanged(e.PropertyName);
        }
    }
}
