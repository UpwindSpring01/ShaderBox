using ShaderBox.General;
using ShaderBox.Models;
using ShaderBox.Models.Annotation;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System;
using System.Collections.Specialized;
using System.ComponentModel;

namespace ShaderBox.UserControls.ViewModels
{
    public class DropdownVecMatPropertyViewModel : UndoRedoBindableBase
    {

        public ObservableCollection<UIElement> Items { get; set; } = new ObservableCollection<UIElement>();

        private AnnotationVariable _annotationVariable;

        public AnnotationVariable AnnotationVariable
        {
            get
            {
                return _annotationVariable;
            }
            set
            {
                _annotationVariable = value;
            
                InitializeItemsSource();
                Values.CollectionChanged += OnCollectionChanged;
                UpdateBuffer();
            }
        }

        private void OnCollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            UpdateBuffer();
        }


        // Wpf doesn't support [,] or [][]
        public ObservableCollection<object> Values
        {
            get
            {
                return _annotationVariable.AnnotationValue.Value as ObservableCollection<object>;
            }
            set
            {
                _annotationVariable.AnnotationValue.Value = value;
            }
        }

        private void UpdateBuffer()
        {
            uint packSize = AnnotationVariable.DataType.GetPackSizeScalar();
            if (AnnotationVariable.IsMatrix && Values[0] is SerializableColor)
            {
                for (int i = 0; i < Values.Count; ++i)
                {
                    uint stepOffset = packSize * AnnotationVariable.Rows;
                    stepOffset = stepOffset.RoundUpMultiple16();
                    AnnotationVariable.CopyColorToBuffer((SerializableColor)Values.ElementAt(i), stepOffset, packSize * (uint)i);
                }
            }
            else
            {
                uint startOffset = 0;
                for (int column = 0; column < AnnotationVariable.Columns; ++column)
                {
                    for (int row = 0; row < AnnotationVariable.Rows; ++row)
                    {
                        if (Values[row * column + column] != null)
                        {
                            AnnotationVariable.CopyToBuffer(PropertyHelpers.ConvertToCorrectType(AnnotationVariable.DataType, Values[row * (int)AnnotationVariable.Columns + column]), startOffset);
                        }
                        startOffset += packSize;
                    }
                    startOffset = startOffset.RoundUpMultiple16();
                }
            }

            AnnotationVariable.AnnotationGroup.MarshalBuffer();
        }

        public DropdownVecMatPropertyViewModel()
        {
            
        }

        private void InitializeItemsSource()
        {
            DataType type = AnnotationVariable.DataType;

            if (type.IsBool())
            {
                if (Values == null || Values.Count != AnnotationVariable.Rows * AnnotationVariable.Columns)
                {
                    Values = new ObservableCollection<object>(new object[AnnotationVariable.Rows * AnnotationVariable.Columns]);
                }

                for (int row = 0; row < AnnotationVariable.Rows; ++row)
                {
                    StackPanel panel = new StackPanel();
                    panel.Orientation = Orientation.Horizontal;
                    for (int column = 0; column < AnnotationVariable.Columns; ++column)
                    {
                        UIElement element = PropertyHelpers.CreateCheckBox(this, $"Values[{row * (int)AnnotationVariable.Columns + column}]", Values[row * (int)AnnotationVariable.Columns + column]);
                        panel.Children.Add(element);
                    }
                    Items.Add(panel);
                }
            }
            else if ((AnnotationVariable.Columns == 3 || AnnotationVariable.Columns == 4) &&
             (AnnotationVariable.Annotations?.Contains(new KeyValuePair<string, string>("UIWidget", "Color")) ?? false))
            {
                if (Values == null || Values.Count != AnnotationVariable.Rows)
                {
                    Values = new ObservableCollection<object>(new object[AnnotationVariable.Rows]);
                }

                for (int row = 0; row < AnnotationVariable.Rows; ++row)
                {
                    UIElement element = PropertyHelpers.CreateColorPicker(this, $"Values[{row}]", (AnnotationVariable.Columns == 4) ? true : false, Values[row]);
                    Items.Add(element);
                }
            }
            else if (AnnotationVariable.Columns == 1 &&
               (AnnotationVariable.Annotations?.Contains(new KeyValuePair<string, string>("UIWidget", "Slider")) ?? false))
            {
                if (Values == null || Values.Count != AnnotationVariable.Rows)
                {
                    Values = new ObservableCollection<object>(new object[AnnotationVariable.Rows]);
                }

                for (int row = 0; row < AnnotationVariable.Rows; ++row)
                {
                    object val = Values[row];
                    if (val == null)
                    {
                        if (AnnotationVariable.DataType.IsIntegral())
                        {
                            Values[row] = 0;
                        }
                        else
                        {
                            Values[row] = 0.0;
                        }
                    }
                    UIElement element = PropertyHelpers.CreateSlider(this, $"Values[{row}]", AnnotationVariable, val);
                    Items.Add(element);
                }
            }
            else
            {
                if (Values == null || Values.Count != AnnotationVariable.Rows * AnnotationVariable.Columns)
                {
                    Values = new ObservableCollection<object>(new object[AnnotationVariable.Rows * AnnotationVariable.Columns]);
                }
                for (int row = 0; row < AnnotationVariable.Rows; ++row)
                {
                    StackPanel panel = new StackPanel();
                    panel.Orientation = Orientation.Horizontal;
                    for (int column = 0; column < AnnotationVariable.Columns; ++column)
                    {
                        UIElement element = PropertyHelpers.CreateSpinner(this, $"Values[{row * (int)AnnotationVariable.Columns + column}]", AnnotationVariable, Values[row * (int)AnnotationVariable.Columns + column]);
                        panel.Children.Add(element);
                    }
                    Items.Add(panel);
                }
            }
        }
    }
}