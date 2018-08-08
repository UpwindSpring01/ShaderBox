using ShaderBox.Converters;
using ShaderBox.Models;
using ShaderBox.Models.Annotation;
using System;
using System.Globalization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Media;
using Xceed.Wpf.Toolkit;

namespace ShaderBox.General
{
    static class PropertyHelpers
    {
        public static bool IsBool(this DataType type)
        {
            return type == DataType.Bool;
        }

        public static bool IsUnsigned(this DataType type)
        {
            switch (type)
            {
                case DataType.Uint:
                case DataType.Min16Uint:
                    return true;
                default:
                    return false;
            }
        }

        public static bool IsIntegral(this DataType type)
        {
            switch (type)
            {
                case DataType.Int:
                case DataType.Uint:
                case DataType.Min16Int:
                case DataType.Min12Int:
                case DataType.Min16Uint:
                    return true;
                default:
                    return false;
            }
        }

        public static bool IsDecimal(this DataType type)
        {
            switch (type)
            {
                case DataType.Half:
                case DataType.Float:
                case DataType.Min16Float:
                case DataType.Min10Float:
                case DataType.Double:
                    return true;
                default:
                    return false;
            }
        }

        public static bool IsMinimum(this DataType type)
        {
            switch (type)
            {
                case DataType.Min16Int:
                case DataType.Min12Int:
                case DataType.Min16Uint:
                case DataType.Min16Float:
                case DataType.Min10Float:
                    return true;
                default:
                    return false;
            }
        }

        public static uint GetPackSizeScalar(this DataType type)
        {
            switch (type)
            {
                case DataType.Int:
                case DataType.Uint:
                case DataType.Float:
                case DataType.Bool:
                case DataType.Min16Int:
                case DataType.Min12Int:
                case DataType.Min16Uint:
                case DataType.Half:
                case DataType.Min16Float:
                case DataType.Min10Float:
                    return 4u;
                case DataType.Double:
                    return 8u;
            }
            return 0u;
        }

        public static object ConvertToCorrectType(DataType dataType, object value)
        {
            object correctedValue = value;
            if (dataType != DataType.Double && value is double && dataType.IsDecimal())
            {
                correctedValue = (float)(double)value;
            }
            else if (dataType == DataType.Uint || dataType == DataType.Min16Uint)
            {
                if (value is int)
                {
                    correctedValue = (uint)(int)value;
                }
                else
                {
                    System.Windows.MessageBox.Show("Error in Packing logic: uint detected but wasn't stored as a uint");
                }
            }
            else if(dataType == DataType.Bool)
            {
                correctedValue = (int)(((bool)value) ? 1 : 0);
            }
            return correctedValue;
        }

        private static SerializableColorConverter _scConverter = new SerializableColorConverter();

        public static UIElement CreateColorPicker(object source, string path, bool useAlpha, object storedValue)
        {
            ColorPicker colorPicker = new ColorPicker();

            colorPicker.ColorMode = ColorMode.ColorCanvas;
            colorPicker.UsingAlphaChannel = useAlpha;

            Binding binding = new Binding();
            binding.Source = source;
            binding.Mode = BindingMode.TwoWay;
            binding.Converter = _scConverter;
            binding.Path = new PropertyPath(path);

            BindingOperations.SetBinding(colorPicker, ColorPicker.SelectedColorProperty, binding);
            if (storedValue == null)
            {
                colorPicker.SelectedColor = Color.FromScRgb(1.0f, 0.8f, 0.8f, 0.8f);
            }
            else
            {
                SerializableColor color = (SerializableColor)storedValue;
                colorPicker.SelectedColor = Color.FromScRgb(color.scA, color.scR, color.scG, color.scB);
            }

            return colorPicker;
        }

        public static UIElement CreateCheckBox(object source, string path, object storedValue)
        {
            CheckBox checkBox = new CheckBox();
            checkBox.VerticalContentAlignment = VerticalAlignment.Center;

            Binding binding = new Binding();
            binding.Source = source;
            binding.Mode = BindingMode.TwoWay;
            binding.Path = new PropertyPath(path);

            BindingOperations.SetBinding(checkBox, ToggleButton.IsCheckedProperty, binding);
            if (storedValue == null)
            {
                checkBox.IsChecked = false;
            }
            else
            {
                checkBox.IsChecked = (bool)storedValue;
            }
            return checkBox;
        }

        public static UIElement CreateSlider(object source, string path, AnnotationVariable var, object storedValue)
        {
            Slider slider = new Slider();
            slider.Minimum = (var.DataType.IsIntegral()) ? var.GetMinimumInteger() : var.GetMinimumDouble();
            slider.Maximum = (var.DataType.IsIntegral()) ? var.GetMaximumInteger() : var.GetMaximumDouble();
            slider.TickFrequency = (var.DataType.IsIntegral()) ? var.GetStepInteger() : var.GetStepDouble();

            slider.IsSnapToTickEnabled = true;
            slider.HorizontalContentAlignment = HorizontalAlignment.Stretch;
            slider.HorizontalAlignment = HorizontalAlignment.Stretch;
            slider.Width = 250;
            Binding binding = new Binding();
            binding.Source = source;
            binding.Mode = BindingMode.TwoWay;
            binding.Path = new PropertyPath(path);

            BindingOperations.SetBinding(slider, RangeBase.ValueProperty, binding);
            if (storedValue == null)
            {
                slider.Value = Math.Max(Math.Min(0.0, slider.Maximum), slider.Minimum);
            }
            else
            {
                slider.Value = (double)storedValue;
            }
            return slider;
        }

        public static UIElement CreateSpinner(object source, string path, AnnotationVariable var, object storedValue)
        {
            if (var.DataType.IsIntegral())
            {
                IntegerUpDown spinner = new IntegerUpDown();
                spinner.Increment = var.GetStepInteger();
                spinner.Minimum = var.GetMinimumInteger();
                spinner.Maximum = var.GetMaximumInteger();
               
                Binding binding = new Binding();
                binding.Source = source;
                binding.Mode = BindingMode.TwoWay;
                binding.Path = new PropertyPath(path);

                BindingOperations.SetBinding(spinner, IntegerUpDown.ValueProperty, binding);
                if (storedValue == null)
                {
                    spinner.Value = Math.Max(Math.Min(0, spinner.Maximum.Value), spinner.Minimum.Value);
                }
                else
                {
                    spinner.Value = (int)storedValue;
                }
                spinner.HorizontalContentAlignment = HorizontalAlignment.Stretch;
                spinner.HorizontalAlignment = HorizontalAlignment.Stretch;

                return spinner;
            }
            else
            {
                DoubleUpDown spinner = new DoubleUpDown();
                spinner.Increment = var.GetStepDouble();
                spinner.Minimum = var.GetMinimumDouble();
                spinner.Maximum = var.GetMaximumDouble();
              

                Binding binding = new Binding();
                binding.Source = source;
                binding.Mode = BindingMode.TwoWay;
                binding.Path = new PropertyPath(path);

                BindingOperations.SetBinding(spinner, DoubleUpDown.ValueProperty, binding);
                if (storedValue == null)
                {
                    spinner.Value = Math.Max(Math.Min(0, spinner.Maximum.Value), spinner.Minimum.Value);
                }
                else
                {
                    spinner.Value = (double)storedValue;
                }
                spinner.HorizontalContentAlignment = HorizontalAlignment.Stretch;
                spinner.HorizontalAlignment = HorizontalAlignment.Stretch;

                return spinner;
            }
        }

        public static int GetMinimumInteger(this AnnotationVariable var)
        {
            int minimum = int.MinValue;
            string min = "";
            if (var.Annotations?.TryGetValue("UIMin", out min) ?? false)
            {
                int.TryParse(min, out minimum);
            }
            if (var.IsSNorm)
            {
                minimum = Math.Max(minimum, -1);
            }
            else if (var.IsUNorm || var.DataType.IsUnsigned())
            {
                minimum = Math.Max(minimum, 0);
            }
            return minimum;
        }

        public static double GetMinimumDouble(this AnnotationVariable var)
        {
            double minimum = double.MinValue;
            string min = "";
            if (var.Annotations?.TryGetValue("UIMin", out min) ?? false)
            {
                double.TryParse(min, NumberStyles.Any, CultureInfo.InvariantCulture, out minimum);
            }
            if (var.IsSNorm)
            {
                minimum = Math.Max(minimum, -1.0);
            }
            else if (var.IsUNorm || var.DataType.IsUnsigned())
            {
                minimum = Math.Max(minimum, 0.0);
            }
            return minimum;
        }

        public static int GetMaximumInteger(this AnnotationVariable var)
        {
            int maximum = int.MaxValue;
            string max = "";
            if (var.Annotations?.TryGetValue("UIMax", out max) ?? false)
            {
                int.TryParse(max, out maximum);
            }
            if (var.IsSNorm || var.IsUNorm)
            {
                maximum = Math.Min(maximum, 1);
            }
            return maximum;
        }

        public static double GetMaximumDouble(this AnnotationVariable var)
        {
            double maximum = double.MaxValue;
            string max = "";
            if (var.Annotations?.TryGetValue("UIMax", out max) ?? false)
            {
                double.TryParse(max, NumberStyles.Any, CultureInfo.InvariantCulture, out maximum);
            }
            if (var.IsSNorm || var.IsUNorm)
            {
                maximum = Math.Min(maximum, 1.0);
            }
            return maximum;
        }

        public static int GetStepInteger(this AnnotationVariable var)
        {
            string uistep = "";
            int step = 1;
            if (var.Annotations?.TryGetValue("UIStep", out uistep) ?? false)
            {
                int.TryParse(uistep, out step);
            }
            return step;
        }

        public static double GetStepDouble(this AnnotationVariable var)
        {
            string uistep = "";
            double step = 0.1;
            if (var.Annotations?.TryGetValue("UIStep", out uistep) ?? false)
            {
                double.TryParse(uistep, NumberStyles.Any, CultureInfo.InvariantCulture, out step);
            }
            return step;
        }
    }
}
