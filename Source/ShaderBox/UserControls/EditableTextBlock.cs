using System.Windows.Controls;
using System.Windows;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Data;
using System;
using ShaderBox.Models;

namespace ShaderBox.UserControls
{
    public class ShaderNameChangedEventArgs : EventArgs
    {
        public string OldValue;
        public string NewValue;
        public Shader Shader;

        public ShaderNameChangedEventArgs(string oldValue, string newValue, Shader shader)
        {
            OldValue = oldValue;
            NewValue = newValue;
            Shader = shader;
        }
    }


    public class EditableTextBlock : TextBlock
    {
        public bool IsInEditMode
        {
            get
            {
                return (bool)GetValue(IsInEditModeProperty);
            }
            set
            {
                SetValue(IsInEditModeProperty, value);
            }
        }

        private EditableTextBlockAdorner _adorner;

        // Using a DependencyProperty as the backing store for IsInEditMode.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsInEditModeProperty =
            DependencyProperty.Register("IsInEditMode", typeof(bool), typeof(EditableTextBlock), new UIPropertyMetadata(false, IsInEditModeUpdate));

        public bool IsEditable
        {
            get
            {
                return (bool)GetValue(IsEditableProperty);
            }
            set
            {
                SetValue(IsEditableProperty, value);
            }
        }

        public static readonly DependencyProperty IsEditableProperty =
          DependencyProperty.Register("IsEditable", typeof(bool), typeof(EditableTextBlock), new UIPropertyMetadata(true, IsInEditModeUpdate));

        public event EventHandler<ShaderNameChangedEventArgs> TextChanged;

        private string _oldValue;
        /// <summary>
        /// Determines whether [is in edit mode update] [the specified obj].
        /// </summary>
        /// <param name="obj">The obj.</param>
        /// <param name="e">The <see cref="System.Windows.DependencyPropertyChangedEventArgs"/> instance containing the event data.</param>
        private static void IsInEditModeUpdate(DependencyObject obj, DependencyPropertyChangedEventArgs e)
        {
            EditableTextBlock textBlock = obj as EditableTextBlock;
            if (null != textBlock)
            {
                //Get the adorner layer of the uielement (here TextBlock)
                AdornerLayer layer = AdornerLayer.GetAdornerLayer(textBlock);

                //If the IsInEditMode set to true means the user has enabled the edit mode then
                //add the adorner to the adorner layer of the TextBlock.
                if (textBlock.IsInEditMode && textBlock.IsEditable)
                {
                    if (null == textBlock._adorner)
                    {
                        textBlock._adorner = new EditableTextBlockAdorner(textBlock);

                        //Events wired to exit edit mode when the user presses Enter key or leaves the control.
                        textBlock._adorner.TextBoxKeyUp += textBlock.TextBoxKeyUp;
                        textBlock._adorner.TextBoxLostFocus += textBlock.TextBoxLostFocus;
                    }
                    layer.Add(textBlock._adorner);
                }
                else
                {
                    //Remove the adorner from the adorner layer.
                    Adorner[] adorners = layer.GetAdorners(textBlock);
                    if (adorners != null)
                    {
                        foreach (Adorner adorner in adorners)
                        {
                            if (adorner is EditableTextBlockAdorner)
                            {
                                layer.Remove(adorner);
                            }
                        }
                    }

                    //Update the textblock's text binding.
                    BindingExpression expression = textBlock.GetBindingExpression(TextProperty);
                    if (null != expression)
                    {
                        expression.UpdateTarget();
                    }
                }
            }
        }

        /// <summary>
        /// Gets or sets the length of the max.
        /// </summary>
        /// <value>The length of the max.</value>
        public int MaxLength
        {
            get
            {
                return (int)GetValue(MaxLengthProperty);
            }
            set
            {
                SetValue(MaxLengthProperty, value);
            }
        }

        // Using a DependencyProperty as the backing store for MaxLength.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty MaxLengthProperty =
            DependencyProperty.Register("MaxLength", typeof(int), typeof(EditableTextBlock), new UIPropertyMetadata(0));

        private void TextBoxLostFocus(object sender, RoutedEventArgs e)
        {
            if (IsInEditMode)
            {
                ExitEditMode();
            }
        }

        /// <summary>
        /// release the edit mode when user presses enter.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="System.Windows.Input.KeyEventArgs"/> instance containing the event data.</param>
        private void TextBoxKeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter && IsInEditMode)
            {
                ExitEditMode();
            }
        }

        private void ExitEditMode()
        {
            IsInEditMode = false;
            if (!Text.Equals(_oldValue))
            {
                string newText = Text;
                Shader shader = (Shader)DataContext;
                TextChanged?.Invoke(this, new ShaderNameChangedEventArgs(_oldValue, newText, shader));
                if (!Text.Equals(shader.Name))
                {
                    Text = shader.Name;
                }
            }
        }


        /// <summary>
        /// Invoked when an unhandled <see cref="E:System.Windows.Input.Mouse.MouseDown"/> attached event reaches an element in its route that is derived from this class. Implement this method to add class handling for this event.
        /// </summary>
        /// <param name="e">The <see cref="T:System.Windows.Input.MouseButtonEventArgs"/> that contains the event data. This event data reports details about the mouse button that was pressed and the handled state.</param>
        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            if (IsEditable)
            {
                if (e.MiddleButton == MouseButtonState.Pressed)
                {
                    IsInEditMode = true;
                    _oldValue = Text;
                }
                else if (e.ClickCount == 2)
                {
                    IsInEditMode = true;
                    _oldValue = Text;
                }
            }
        }
    }
}