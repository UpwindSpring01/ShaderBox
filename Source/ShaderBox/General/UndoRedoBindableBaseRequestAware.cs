using Prism.Commands;
using Prism.Interactivity.InteractionRequest;
using Prism.Mvvm;
using ShaderBox.UndoRedo;
using System;
using System.ComponentModel;
using System.Windows.Input;

namespace ShaderBox.General
{
    class UndoRedoBindableBaseRequestAware<T> : BindableBase, IInteractionRequestAware where T : INotification
    {
        public ICommand RaiseKeyDownCommand { get; private set; }

        public UndoRedoBindableBaseRequestAware()
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

        protected void SetValue(object model, object value, string propName)
        {
            UndoRedoPropertyItem propItem = new UndoRedoPropertyItem();
            propItem.ModelObject = model;
            propItem.PropertyStr = propName;
            propItem.NewValue = value;
            propItem.DoCommand();
            UndoRedoSystem.AddToUndoStack(propItem);
        }

        protected void SetValueMulti(object model, params Tuple<object, string>[] props)
        {
            UndoRedoMultiPropertyItem multiPropItem = new UndoRedoMultiPropertyItem();
            foreach(Tuple<object, string> prop in props)
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

        protected T _notification;

        public Action FinishInteraction
        {
            get;
            set;
        }

        public INotification Notification
        {
            get
            {
                return _notification;
            }

            set
            {
                SetProperty(ref _notification, (T)value);
            }
        }
    }
}
