using ShaderBox.ViewModels;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Collections.ObjectModel;
using FirstFloor.ModernUI.Windows.Controls;

namespace ShaderBox.Views
{
    /// <summary>
    /// Interaction logic for NewShaderPage.xaml
    /// </summary>
    public partial class ShaderEditorPage : ModernFrame
    {
        private class DefinitionStorage
        {
            public DefinitionStorage(double actual, double minimum, DefinitionBase definition)
            {
                Actual = actual;
                Minimum = minimum;
                Definition = definition;
            }

            public DefinitionBase Definition { get; private set; }

            public double Actual { get; set; }
            public double Minimum { get; private set; }
        }

        private class DefinitionDictionary : KeyedCollection<DefinitionBase, DefinitionStorage>
        {
            protected override DefinitionBase GetKeyForItem(DefinitionStorage item)
            {
                return item.Definition;
            }
        }

        private readonly int MinimumWidthExplorer = 150;
        private readonly int MinimumWidthCodeView = 200;
        private readonly int MinimumWidthViewportShaderProp = 200;
        private readonly int DimensionCollapsedExpander = 38;
        private readonly int GridSplitterWidth = 10;

        private DefinitionDictionary _columnWidthMap = new DefinitionDictionary();

        private double _errorRowHeight = -1;
        private double _shaderPropertiesHeight = -1;
        private double _viewportHeight = -1;

        private Grid _explorerCodeViewGrid = null;

        private bool _loaded = false;

        public ShaderEditorPage()
        {
            InitializeComponent();

            InitializeTextEditor();
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            (DataContext as ShaderEditorPageViewModel).TextEditor = textEditor;
            _loaded = true;

            _columnWidthMap.Clear();
            _columnWidthMap.Add(new DefinitionStorage(explorerColumn.ActualWidth, MinimumWidthExplorer, explorerColumn));
            _columnWidthMap.Add(new DefinitionStorage(codeViewColumn.ActualWidth, MinimumWidthCodeView, codeViewColumn));
            _columnWidthMap.Add(new DefinitionStorage(shaderViewPropColumn.ActualWidth, MinimumWidthViewportShaderProp, shaderViewPropColumn));
            _errorRowHeight = errorRow.ActualHeight;
            _shaderPropertiesHeight = shaderPropertiesRow.ActualHeight;
            _viewportHeight = viewportRow.ActualHeight;

            explorerColumn.Width = new GridLength(_columnWidthMap[explorerColumn].Actual, GridUnitType.Star);
            codeViewColumn.Width = new GridLength(_columnWidthMap[codeViewColumn].Actual, GridUnitType.Star);
            shaderViewPropColumn.Width = new GridLength(_columnWidthMap[shaderViewPropColumn].Actual, GridUnitType.Star);

            errorRow.Height = new GridLength(_errorRowHeight, GridUnitType.Star);
            shaderPropertiesRow.Height = new GridLength(_shaderPropertiesHeight, GridUnitType.Star);
            viewportRow.Height = new GridLength(_viewportHeight, GridUnitType.Star);

            editorRow.Height = new GridLength(editorRow.ActualHeight, GridUnitType.Star);
        }

        private void explorerExpander_Collapsed(object sender, RoutedEventArgs e)
        {
            Collapse(explorerColumn);
            e.Handled = true;
        }

        private void explorerExpander_Expanded(object sender, RoutedEventArgs e)
        {
            Expand(explorerColumn);
            e.Handled = true;
        }

        private void errorExpander_Collapsed(object sender, RoutedEventArgs e)
        {
            _errorRowHeight = errorRow.ActualHeight;
            errorRow.MinHeight = DimensionCollapsedExpander;
            errorRow.MaxHeight = DimensionCollapsedExpander;
            errorRow.Height = new GridLength(DimensionCollapsedExpander, GridUnitType.Star);
            e.Handled = true;
        }

        private void errorExpander_Expanded(object sender, RoutedEventArgs e)
        {
            if (_loaded)
            {
                errorRow.MinHeight = 90;
                errorRow.MaxHeight = double.PositiveInfinity;
                errorRow.Height = new GridLength(_errorRowHeight, GridUnitType.Star);
                e.Handled = true;
            }
        }

        private void codeViewExpander_Collapsed(object sender, RoutedEventArgs e)
        {
            Collapse(codeViewColumn);
            e.Handled = true;
        }

        private void codeViewExpander_Expanded(object sender, RoutedEventArgs e)
        {
            Expand(codeViewColumn);
            e.Handled = true;
        }

        private void shaderPropertiesExpander_Collapsed(object sender, RoutedEventArgs e)
        {
            _shaderPropertiesHeight = shaderPropertiesRow.ActualHeight;
            shaderPropertiesRow.MinHeight = DimensionCollapsedExpander;
            shaderPropertiesRow.MaxHeight = DimensionCollapsedExpander;
            shaderPropertiesRow.Height = new GridLength(DimensionCollapsedExpander, GridUnitType.Star);
            e.Handled = true;
        }

        private void shaderPropertiesExpander_Expanded(object sender, RoutedEventArgs e)
        {
            if (_loaded)
            {
                shaderPropertiesRow.MinHeight = 200;
                shaderPropertiesRow.MaxHeight = double.PositiveInfinity;
                shaderPropertiesRow.Height = new GridLength(_shaderPropertiesHeight, GridUnitType.Star);
                e.Handled = true;
            }
        }

        private void shaderViewPropExpander_Collapsed(object sender, RoutedEventArgs e)
        {
            Collapse(shaderViewPropColumn);
            e.Handled = true;
        }

        private void shaderViewPropExpander_Expanded(object sender, RoutedEventArgs e)
        {
            Expand(shaderViewPropColumn);
            e.Handled = true;
        }

        private void viewportExpander_Collapsed(object sender, RoutedEventArgs e)
        {
            _viewportHeight = viewportRow.ActualHeight;
            viewportRow.MinHeight = DimensionCollapsedExpander;
            viewportRow.MaxHeight = DimensionCollapsedExpander;
            viewportRow.Height = new GridLength(DimensionCollapsedExpander, GridUnitType.Star);
            e.Handled = true;
        }

        private void viewportExpander_Expanded(object sender, RoutedEventArgs e)
        {
            if (_loaded)
            {
                viewportRow.MinHeight = 200;
                viewportRow.MaxHeight = double.PositiveInfinity;
                viewportRow.Height = new GridLength(_viewportHeight, GridUnitType.Star);
                e.Handled = true;
            }
        }

        private void Expander_SetHandled(object sender, RoutedEventArgs e)
        {
            e.Handled = true;
        }

        private void Collapse(ColumnDefinition col)
        {
            if (col.MaxWidth != 0)
            {
                _columnWidthMap[col].Actual = col.Width.Value;
            }

            col.MinWidth = DimensionCollapsedExpander;
            col.MaxWidth = DimensionCollapsedExpander;

            // If last column, add new width to previous column if expanded
            // If first column, add new width to next column if expanded
            // Afterwards let WPF handle it
            int index = _columnWidthMap.IndexOf(_columnWidthMap[col]);
            DefinitionStorage definition = (index < _columnWidthMap.Count - 1) ? _columnWidthMap[index + 1] : _columnWidthMap[index - 1];
            ColumnDefinition colDef = (ColumnDefinition)definition.Definition;

            double width = _columnWidthMap[col].Actual;
            // Check if expanded
            if (colDef.MaxWidth != DimensionCollapsedExpander)
            {
                double newWidth = colDef.Width.Value + (width - DimensionCollapsedExpander);
                colDef.Width = new GridLength(newWidth, GridUnitType.Star);
            }

            col.Width = new GridLength(DimensionCollapsedExpander, GridUnitType.Star);
            CheckExploreCodeViewGrid();
        }

        private void Expand(ColumnDefinition col)
        {
            // Filter out expand events at load and when is created or destroyed ExplorerCodeViewGrid
            if (!_loaded || col.MaxWidth == 0 || (double.IsPositiveInfinity(col.MaxWidth) && _explorerCodeViewGrid == null))
            {
                return;
            }
            col.MinWidth = _columnWidthMap[col].Minimum;
            col.MaxWidth = double.PositiveInfinity;

            // If not last column, substract new width from the next column if expanded
            // If last column substract from previous column if expanded
            // Afterwards let WPF handle it
            int index = _columnWidthMap.IndexOf(_columnWidthMap[col]);
            DefinitionStorage definition = (index < _columnWidthMap.Count - 1) ? _columnWidthMap[index + 1] : _columnWidthMap[index - 1];
            ColumnDefinition colDef = (ColumnDefinition)definition.Definition;

            double width = _columnWidthMap[col].Actual;
            // Check if expanded
            if (colDef.MaxWidth != DimensionCollapsedExpander)
            {
                double newWidth = Math.Max(colDef.Width.Value - (width - DimensionCollapsedExpander), definition.Minimum);
                colDef.Width = new GridLength(newWidth, GridUnitType.Star);
            }

            col.Width = new GridLength(width, GridUnitType.Star);
            CheckExploreCodeViewGrid();
        }

        private void CheckExploreCodeViewGrid()
        {
            if (_explorerCodeViewGrid == null)
            {
                if (explorerExpander.IsExpanded && shaderViewPropExpander.IsExpanded && !codeViewExpander.IsExpanded)
                {
                    CreateExplorerCodeViewGrid();
                }
            }
            else
            {
                if ((!explorerExpander.IsExpanded || !shaderViewPropExpander.IsExpanded) ||
                    codeViewExpander.IsExpanded)
                {
                    DestroyExplorerCodeViewGrid();
                }
            }
        }

        // When codeview is collapsed and explorer and shaderview are expanded
        // Move the collapsed codeview expander and expanded explorer to a new grid and place this in the third column.
        // So the gridsplitter affects the explorer column
        private void CreateExplorerCodeViewGrid()
        {
            if (explorerColumn.MaxWidth != DimensionCollapsedExpander)
            {
                _columnWidthMap[explorerColumn].Actual = explorerColumn.Width.Value;
            }
            _explorerCodeViewGrid = new Grid();

            // Move the explorerexpander to the inner grid
            ColumnDefinition newCol = new ColumnDefinition();
            newCol.Width = new GridLength(_columnWidthMap[explorerColumn].Actual, GridUnitType.Star);
            newCol.MinWidth = MinimumWidthExplorer;
            newCol.MaxWidth = double.PositiveInfinity;
            _explorerCodeViewGrid.ColumnDefinitions.Add(newCol);

            mainGrid.Children.Remove(explorerExpander);
            _explorerCodeViewGrid.Children.Add(explorerExpander);

            // Move the collapsed codeviewexpander to the inner grid
            newCol = new ColumnDefinition();
            newCol.Width = new GridLength(DimensionCollapsedExpander, GridUnitType.Star);
            newCol.MinWidth = DimensionCollapsedExpander;
            newCol.MaxWidth = DimensionCollapsedExpander;
            _explorerCodeViewGrid.ColumnDefinitions.Add(newCol);

            mainGrid.Children.Remove(codeViewExpander);
            _explorerCodeViewGrid.Children.Add(codeViewExpander);

            // Set new values for outer grid
            explorerColumn.Width = new GridLength(0, GridUnitType.Star);
            explorerColumn.MinWidth = 0;
            explorerColumn.MaxWidth = 0;

            codeViewColumn.MinWidth = MinimumWidthExplorer + DimensionCollapsedExpander;
            codeViewColumn.MaxWidth = double.PositiveInfinity;

            // Keep explorer column at it original size
            codeViewColumn.Width = new GridLength(_columnWidthMap[explorerColumn].Actual + DimensionCollapsedExpander + GridSplitterWidth, GridUnitType.Star);

            // Attach to grid and colums
            Grid.SetColumn(explorerExpander, 0);
            Grid.SetColumn(codeViewExpander, 1);

            mainGrid.Children.Add(_explorerCodeViewGrid);

            Grid.SetColumn(_explorerCodeViewGrid, 2);
        }

        private void DestroyExplorerCodeViewGrid()
        {
            _explorerCodeViewGrid.Children.Remove(explorerExpander);
            _explorerCodeViewGrid.Children.Remove(codeViewExpander);
            mainGrid.Children.Remove(_explorerCodeViewGrid);
            mainGrid.Children.Add(explorerExpander);
            mainGrid.Children.Add(codeViewExpander);

            Grid.SetColumn(explorerExpander, 0);
            Grid.SetColumn(codeViewExpander, 2);

            if (explorerExpander.IsExpanded)
            {
                explorerColumn.MinWidth = _columnWidthMap[explorerColumn].Minimum;
                explorerColumn.MaxWidth = double.PositiveInfinity;
                explorerColumn.Width = new GridLength(_columnWidthMap[explorerColumn].Actual, GridUnitType.Star);
            }
            else
            {
                explorerColumn.MinWidth = DimensionCollapsedExpander;
                explorerColumn.MaxWidth = DimensionCollapsedExpander;
                explorerColumn.Width = new GridLength(DimensionCollapsedExpander, GridUnitType.Star);
            }

            if (codeViewExpander.IsExpanded)
            {
                codeViewColumn.MinWidth = _columnWidthMap[codeViewColumn].Minimum;
                codeViewColumn.MaxWidth = double.PositiveInfinity;
                codeViewColumn.Width = new GridLength(_columnWidthMap[codeViewColumn].Actual, GridUnitType.Star);
            }
            else
            {
                codeViewColumn.MinWidth = DimensionCollapsedExpander;
                codeViewColumn.MaxWidth = DimensionCollapsedExpander;
                codeViewColumn.Width = new GridLength(DimensionCollapsedExpander, GridUnitType.Star);
            }

            _explorerCodeViewGrid = null;
        }

        private void root_MouseDown(object sender, MouseButtonEventArgs e)
        {
            root.Focus();
        }
    }
}