using FirstFloor.ModernUI.Windows.Controls;
using ICSharpCode.AvalonEdit;
using Prism.Commands;
using Prism.Events;
using Prism.Interactivity.InteractionRequest;
using ShaderBox.Events;
using ShaderBox.General;
using ShaderBox.Models;
using ShaderBox.Models.Annotation;
using ShaderBox.Notifications;
using ShaderBox.UserControls;
using ShaderBoxBridge;
using ShaderTools.CodeAnalysis.Hlsl.Syntax;
using ShaderTools.CodeAnalysis.Syntax;
using ShaderTools.CodeAnalysis.Text;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Threading;

namespace ShaderBox.ViewModels
{
    public partial class ShaderEditorPageViewModel : UndoRedoBindableBase, ITempHelper
    {
        private const string PATH_TO_SHARED_FOLDER = "./Resources/Shared/";

        private ShaderStorageLinker _shaderStorage;
        public Workspace Workspace
        {
            get;
            private set;
        }

        public ObservableCollection<Shader> ShadersFlattened
        {
            get;
            private set;
        } = new ObservableCollection<Shader>();

        public ICollectionView ShaderGroupsView
        {
            get;
            private set;
        }

        public int SelectedIndex
        {
            get
            {
                return Workspace.SelectedIndex;
            }
            set
            {
                SetValue(Workspace, value, () =>
                {
                    RaiseCompileShaderCommand.RaiseCanExecuteChanged();
                    RaiseBuildShaderGroupCommand.RaiseCanExecuteChanged();
                    SelectionChanged();
                });
            }
        }

        public Model3DData SelectedModel
        {
            get
            {
                return Workspace.Models.FirstOrDefault((m) => m.Hash == (SelectedShaderGroup?.SelectedModel ?? 0));
            }
            set
            {
                if (SelectedShaderGroup != null)
                {
                    SetValue(SelectedShaderGroup, value.Hash, () => ViewportHost.SetModel(SelectedModel.Path));
                }
            }
        }


        private ViewportHost _viewportHost;
        public ViewportHost ViewportHost
        {
            get
            {
                return _viewportHost;
            }
            set
            {
                _viewportHost = value;
                UpdateShaderViewport();
            }
        }

        private float _cameraOffset;
        public float CameraOffset
        {
            get { return _cameraOffset; }
            set
            {
                _cameraOffset = value;
                ViewportHost?.SetCameraOffset(value);
            }
        }

        public bool IsStandardShaderActive => SelectedShaderGroup.ShaderGroupType == ShaderGroupType.Standard;

        private static string _errorPattern = @"\([0-9]+,[0-9]+(-[0-9]+)?\):";
        private static string _numberPattern = @"[0-9]+";

        public TextEditor TextEditor { get; set; }


        public ICommand SelectErrorCommand { get; private set; }
        public ICommand RaiseNewShaderGroupCommand { get; private set; }

        public ICommand RaiseLoadedCommand { get; private set; }

        public DelegateCommand RaiseCompileShaderCommand { get; private set; }
        public DelegateCommand RaiseBuildShaderGroupCommand { get; set; }

        public InteractionRequest<ShaderGroupNotification> ShaderGroupRequest { get; private set; } = new InteractionRequest<ShaderGroupNotification>();

        public delegate void CallbackDelegate(string fileName);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void setCallback(IntPtr aCallback);

        private CallbackDelegate _includeHeaderCallback;

        private ShaderGroup _selectedShaderGroup;
        public ShaderGroup SelectedShaderGroup
        {
            get
            {
                return _selectedShaderGroup;
            }
            set
            {
                SetProperty(ref _selectedShaderGroup, value);
            }
        }

        private Shader CompilingShader { get; set; }

        private ShaderGroup _settedGroup = null;

        public ShaderEditorPageViewModel(ShaderStorageLinker shaderStorage, Workspace workspace, EventAggregator eventAggregator)
        {
            int index = workspace.SelectedIndex;

            _shaderStorage = shaderStorage;
            Workspace = workspace;

            ShadersFlattened.AddRange(_shaderStorage.ShaderGroups.SelectMany((s) => s.Shaders));
            ShaderGroupsView = CollectionViewSource.GetDefaultView(ShadersFlattened);
            ShaderGroupsView.Filter = (s) => ((Shader)s).Group.IsOpen;
            ShaderGroupsView.GroupDescriptions.Add(new PropertyGroupDescription("Group"));
            RaiseLoadedCommand = new DelegateCommand(RaiseLoaded);

            SelectErrorCommand = new DelegateCommand<SelectionChangedEventArgs>(SelectError);
            RaiseCompileShaderCommand = new DelegateCommand(RaiseCompileShader, CanExecuteCompileShader);
            RaiseNewShaderGroupCommand = new DelegateCommand(RaiseNewShaderGroup);

            RaiseShaderNameChangedCommand = new DelegateCommand<ShaderNameChangedEventArgs>(RaiseShaderNameChanged);

            InitializeExplorerButtons();
            InitializeSaveButtonCommands();

            RaiseBuildShaderGroupCommand = new DelegateCommand(RaiseBuildShaderGroup, CanExecuteCompileShader);

            _includeHeaderCallback = new CallbackDelegate(SaveIncludeHeaderCallback);

            Workspace.SelectedIndex = index;

            eventAggregator.GetEvent<RefreshExplorer>().Subscribe(RefreshExplorerEvent);
        }

        private void UpdateShaderViewport()
        {
            if (ViewportHost != null)
            {
                if (SelectedShaderGroup != null && SelectedShaderGroup.IsBuilded)
                {
                    string[] shaderLocations = new string[5];
                    foreach (Shader s in SelectedShaderGroup.Shaders)
                    {
                        if (s.ShaderType != ShaderType.Header)
                        {
                            shaderLocations[(int)s.ShaderType] = $"{MainWindowPageViewModel.ShaderBoxResourcesFolderLocation}{SelectedShaderGroup.UniqueName}/.cso/{s.GetShaderTarget().Substring(0, 2)}.cso";
                        }
                    }
                    bool isStandard = SelectedShaderGroup.ShaderGroupType == ShaderGroupType.Standard;
                    if (isStandard)
                    {
                        ViewportHost.SetShaders(shaderLocations[0] ?? "", shaderLocations[1] ?? "", shaderLocations[2] ?? "", shaderLocations[3] ?? "", shaderLocations[4] ?? "");
                        ViewportHost.SetTopology((int)SelectedShaderGroup.Topology);
                        ViewportHost.SetRasterizerState((int)SelectedShaderGroup.CullMode, (int)SelectedShaderGroup.FillMode);
                    }
                    else
                    {
                        ViewportHost.SetPPShader(shaderLocations[4] ?? "");
                    }
                    _settedGroup = SelectedShaderGroup;

                    foreach (AnnotationShaderGroup annotationShaderGroup in SelectedShaderGroup.AnnotationShaderGroups)
                    {
                        foreach (AnnotationGroup annotationGroup in annotationShaderGroup.Buffers)
                        {
                            foreach (AnnotationVariable annotationVariable in annotationGroup.AnnotationVariables)
                            {
                                annotationVariable.UpdateBuffer(false);
                            }
                            annotationGroup.MarshalBuffer(ViewportHost, isStandard);
                        }
                    }
                }

                ViewportHost.SetModel(SelectedModel?.Path ?? Workspace.Models[0].Path);
                ViewportHost.SetCameraOffset(CameraOffset);

                RaisePropertyChanged("SelectedModel");
            }
        }

        private void RaiseLoaded()
        {
            if (((ModernWindow)Application.Current.MainWindow).ContentSource.ToString() == "/ShaderBox;component/Views/ShaderEditorPage.xaml")
            {
                ((App)Application.Current).ActiveViewport = this;
            }
        }

        private void SelectionChanged()
        {
            if (SelectedIndex >= 0)
            {
                Shader shader = ShaderGroupsView.Cast<Shader>().ElementAt(SelectedIndex);
                if (SelectedShaderGroup != shader.Group)
                {
                    SelectedShaderGroup = shader.Group;
                    UpdateShaderViewport();
                }
            }
            else
            {
                SelectedShaderGroup = null;
            }
        }

        private void RefreshExplorerEvent(ShaderGroup shaderGroup)
        {
            if (shaderGroup != null)
            {
                foreach (Shader shader in shaderGroup.Shaders)
                {
                    ShadersFlattened.Remove(shader);
                }
            }
            ShaderGroupsView.Refresh();
        }

        private bool CanExecuteCompileShader()
        {
            return !(((Shader)ShaderGroupsView.CurrentItem)?.ShaderType == ShaderType.Header);
        }

        private void RaiseShaderNameChanged(ShaderNameChangedEventArgs e)
        {
            SetNewShaderName(e);
            if (!e.OldValue.Equals(e.Shader.Name))
            {
                e.Shader.Move(Workspace);
            }
        }

        private void SetNewShaderName(ShaderNameChangedEventArgs e)
        {
            string newValue = e.NewValue;
            string oldValue = e.OldValue;
            Shader shader = e.Shader;
            string extension;
            if (shader.ShaderType == ShaderType.Header)
            {
                extension = ".hlsli";
            }
            else
            {
                extension = ".hlsl";
            }

            // Check if new name is a valid filename, if not reset to old
            try
            {
                if (string.IsNullOrWhiteSpace(newValue) || newValue.LastIndexOf('.') == 0 || !newValue.Equals(Path.GetFileName(newValue)))
                {
                    shader.Name = oldValue;
                    return;
                }
            }
            catch (Exception)
            {
                shader.Name = oldValue;
                return;
            }

            // Check if it ends with the correct extension, if not add it
            if (!newValue.EndsWith(extension))
            {
                int startPos = newValue.LastIndexOf('.');
                if (startPos == -1)
                {
                    startPos = newValue.Length;
                }
                newValue = newValue.Substring(0, startPos) + extension;
            }

            // If filename already in use add a counter
            IEnumerable<string> shaders = shader.Group.Shaders.Where((s) => s != shader).Select((s) => s.Name);
            string filename = Path.GetFileNameWithoutExtension(newValue);
            int counter = 0;
            while (shaders.Contains(newValue))
            {
                ++counter;
                newValue = $"{filename}_{counter.ToString("D3")}{extension}";
                if (counter == 999)
                {
                    // Unable to find free name
                    shader.Name = oldValue;
                    return;
                }
            }
            shader.Name = newValue;

            shader.Group.Save(Workspace);
        }

        private void RaiseNewShaderGroup()
        {
            ShaderGroupNotification notification = new ShaderGroupNotification();
            notification.Title = "New Shader";
            notification.Content = null;
            ShaderGroupRequest.Raise(notification, (returned) =>
            {
                if (returned.Confirmed)
                {
                    notification.ShaderGroup.IsOpen = true;
                    _shaderStorage.ShaderGroups.Add(notification.ShaderGroup);
                    ShadersFlattened.AddRange(notification.ShaderGroup.Shaders);

                    notification.ShaderGroup.Save(Workspace);
                    Workspace.Save();
                }
            });
        }

        private void SelectError(SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count > 0)
            {
                string error = e.AddedItems[0].ToString();
                Match match = Regex.Match(error, _errorPattern);
                if (match.Success)
                {
                    MatchCollection numbers = Regex.Matches(match.Value, _numberPattern);
                    TextEditor.TextArea.Caret.Line = Convert.ToInt32(numbers[0].Value);
                    TextEditor.TextArea.Caret.Column = Convert.ToInt32(numbers[1].Value);
                    TextEditor.Dispatcher.BeginInvoke(DispatcherPriority.Input, new ThreadStart(() =>
                    {
                        Keyboard.Focus(TextEditor);
                        TextEditor.TextArea.Caret.BringCaretToView();
                    }));
                }

            }
        }

        public void SaveIncludeHeaderCallback(string fileName)
        {
            // Needed for supporting ./../.Shared and ../.Shared etc
            string fullPath = Path.GetFullPath(Path.Combine(MainWindowPageViewModel.ShaderBoxResourcesFolderLocation, CompilingShader.Group.UniqueName, fileName));
            string directoryPath = Path.GetDirectoryName(fullPath);

            // Shared shader
            if (Path.GetFullPath(MainWindowPageViewModel.ShaderBoxSharedFolderLocation).Equals(directoryPath + "\\"))
            {
                RaiseSaveDocument(_shaderStorage.ShaderGroups[0].Shaders.FirstOrDefault((s) => s.Name == Path.GetFileName(fileName)));
            }
            else
            {
                RaiseSaveDocument(CompilingShader.Group.Shaders.FirstOrDefault((s) => s.Name == Path.GetFileName(fileName)));
            }
        }

        private void RaiseBuildShaderGroup()
        {
            Shader shader = ShaderGroupsView.CurrentItem as Shader;
            if (shader.Group.ShaderGroupType != ShaderGroupType.SharedHeaders)
            {
                shader.Group.AnnotationShaderGroups.Clear();

                bool hasErrors = false;
                string[] shaderLocations = new string[5];
                foreach (Shader s in shader.Group.Shaders)
                {
                    if (s.ShaderType != ShaderType.Header)
                    {
                        hasErrors |= CompileShader(s, true);
                        shaderLocations[(int)s.ShaderType] = $"{MainWindowPageViewModel.ShaderBoxResourcesFolderLocation}{shader.Group.UniqueName}/.cso/{s.GetShaderTarget().Substring(0, 2)}.cso";
                        if (hasErrors)
                        {
                            shader.Group.IsBuilded = false;
                            if (Directory.Exists(Path.GetDirectoryName(Path.GetFullPath(s.FileLocation)) + "/.cso"))
                            {
                                Directory.Delete(Path.GetDirectoryName(Path.GetFullPath(s.FileLocation)) + "/.cso", true);
                            }
                            break;
                        }
                    }
                }

                // Only refresh shader variables if compilation of all shaders succesfull

                if (!hasErrors)
                {
                    SelectedShaderGroup.IsBuilded = true;

                    if (SelectedShaderGroup.ShaderGroupType == ShaderGroupType.Standard)
                    {
                        if(SelectedShaderGroup == _settedGroup)
                        {
                            ViewportHost.UpdateShaders(
                                shaderLocations[0] ?? "", shaderLocations[1] ?? "", shaderLocations[2] ?? "", shaderLocations[3] ?? "", shaderLocations[4] ?? "");
                        }
                        else
                        {
                            // Workaround so we don't update the wrong shader, if shaders wasn't in cache we load everything double, not good
                            ViewportHost.SetShaders(
                                shaderLocations[0] ?? "", shaderLocations[1] ?? "", shaderLocations[2] ?? "", shaderLocations[3] ?? "", shaderLocations[4] ?? "");
                            ViewportHost.UpdateShaders(
                                shaderLocations[0] ?? "", shaderLocations[1] ?? "", shaderLocations[2] ?? "", shaderLocations[3] ?? "", shaderLocations[4] ?? "");
                            ViewportHost.SetTopology((int)SelectedShaderGroup.Topology);
                            ViewportHost.SetRasterizerState((int)SelectedShaderGroup.CullMode, (int)SelectedShaderGroup.FillMode);
                        }
                    }
                    else
                    {
                        if (SelectedShaderGroup == _settedGroup)
                        {
                            ViewportHost.UpdatePPShader(shaderLocations[4] ?? "");
                        }
                        else
                        {
                            ViewportHost.SetPPShader(shaderLocations[4] ?? "");
                            ViewportHost.UpdatePPShader(shaderLocations[4] ?? "");
                        }
                    }
                    try
                    {
                        foreach (Shader s in SelectedShaderGroup.Shaders)
                        {

                            SyntaxTree syntaxTree = SyntaxFactory.ParseSyntaxTree(SourceText.From(s.Document.Text), null, null);
                            AnnotationShaderGroup options = UpdateVariables(syntaxTree, s.ShaderType);
                            if (options == null)
                            {
                                break;
                            }
                            if (options.Buffers.Count > 0)
                            {
                                SelectedShaderGroup.AnnotationShaderGroups.Add(options);
                            }
                        }

                        ViewportHost.RenderThumbnail(Path.GetFullPath($"{MainWindowPageViewModel.ShaderBoxResourcesFolderLocation}{SelectedShaderGroup.UniqueName}/preview.png"),
                            async () =>
                            {
                                await Application.Current.Dispatcher.InvokeAsync(() =>
                                {
                                    _selectedShaderGroup.Image = null;
                                    ViewportHost.PopCallbackRenderThumbnail();
                                });
                            });
                    }
                    catch (Exception)
                    {
                        SelectedShaderGroup.AnnotationShaderGroups.Clear();
                    }
                }
                SelectedShaderGroup.Save(Workspace);
            }
        }

        private void RaiseCompileShader()
        {
            CompileShader(ShaderGroupsView.CurrentItem as Shader, false);
        }

        private bool CompileShader(Shader shader, bool saveToDisk)
        {
            CompilingShader = shader;
            RaiseSaveDocument(CompilingShader);

            CompilingShader.Errors.Clear();

            IntPtr ptr = NativeMethods.CompileShader(CompilingShader.Document.Text, CompilingShader.GetShaderTarget(),
                Path.GetDirectoryName(Path.GetFullPath(CompilingShader.FileLocation)), saveToDisk, Marshal.GetFunctionPointerForDelegate(_includeHeaderCallback));
            string e = Marshal.PtrToStringAnsi(ptr);

            bool hasErrors = !string.IsNullOrWhiteSpace(e);
            if (hasErrors)
            {
                string[] errors = e.Split('\n');
                foreach (string error in errors)
                {
                    if (!string.IsNullOrWhiteSpace(error))
                    {
                        CompilingShader.Errors.Add(error);
                    }
                }
                ShaderGroupsView.MoveCurrentTo(shader);

            }
            NativeMethods.DeleteArray(ref ptr);
            CompilingShader = null;

            return hasErrors;
        }

        private AnnotationShaderGroup UpdateVariables(SyntaxTree tree, ShaderType header)
        {
            // Probably should use a combined approach of the compiled shader with the source for annotations to get this info.
            //
            // because now it will also show unused buffers/variables
            // Also when you register buffers to the same slot and use only one, the shader will compile and work, but I don't have
            // any idea about which buffer I need to bind to the pipeline, so current solution show errorbox
            AnnotationFactory.Start(header);
            IList<SyntaxNodeBase> nodes = tree.Root.ChildNodes;
            List<string> registers = new List<string>();
            foreach (SyntaxNodeBase node in nodes)
            {
                if (node is ConstantBufferSyntax)
                {
                    ConstantBufferSyntax structType = (ConstantBufferSyntax)node;
                    string register = structType.Register.Register.ValueText;

                    AnnotationFactory.BeginGroup();
                    if (registers.Contains(register))
                    {
                        MessageBox.Show($"Register: {register}\nDefined multiple times, ShaderBox can not decides which one needs to be binded.\n\nWill be resolved in a future release.", "Unsupported operation", MessageBoxButton.OK, MessageBoxImage.Error);
                        AnnotationFactory.DestroyGroup();
                        return null;
                    }
                    registers.Add(register);
                    AnnotationFactory.SetRegister(register);
                    foreach (VariableDeclarationStatementSyntax syntax in structType.Declarations)
                    {
                        if (!ParseVariableDeclarationStatementSyntax(syntax, registers))
                        {
                            return null;
                        }
                    }
                    AnnotationFactory.EndGroup();
                }

                if (node is VariableDeclarationStatementSyntax)
                {
                    AnnotationFactory.BeginGroup();
                    if (!ParseVariableDeclarationStatementSyntax((VariableDeclarationStatementSyntax)node, registers))
                    {
                        return null;
                    }
                    AnnotationFactory.EndGroup();
                }
            }

            return AnnotationFactory.End();
        }

        private bool ParseVariableDeclarationStatementSyntax(VariableDeclarationStatementSyntax syntax, List<string> registers)
        {
            VariableDeclarationSyntax varDeclaration = syntax.Declaration;

            TypeSyntax type = varDeclaration.Type;
            foreach (VariableDeclaratorSyntax var in varDeclaration.Variables)
            {
                AnnotationFactory.BeginVar();
                AnnotationFactory.SetModifiers(varDeclaration.Modifiers);

                if (AnnotationFactory.IsGroupOpen())
                {
                    AnnotationFactory.SetType(type);
                    AnnotationFactory.SetAnnotations(var.Annotations?.Annotations ?? null);
                    InitializerSyntax initializer = var.Initializer;
                    if (varDeclaration.Variables[0].ArrayRankSpecifiers.Count > 0)
                    {
                        LiteralExpressionSyntax litExpressionSyntax = varDeclaration.Variables[0].ArrayRankSpecifiers[0].Dimension as LiteralExpressionSyntax;
                        AnnotationFactory.SetDimension((int)litExpressionSyntax.Token.Value);
                    }
                    if (initializer != null && initializer is EqualsValueClauseSyntax)
                    {
                        EqualsValueClauseSyntax evcSyntax = (EqualsValueClauseSyntax)initializer;
                        if (evcSyntax.Value is NumericConstructorInvocationExpressionSyntax)
                        {
                            NumericConstructorInvocationExpressionSyntax ncieSyntax = (NumericConstructorInvocationExpressionSyntax)evcSyntax.Value;
                            SeparatedSyntaxList<ExpressionSyntax> arguments = ncieSyntax.ArgumentList.Arguments;
                        }
                        else if (evcSyntax.Value is LiteralExpressionSyntax)
                        {

                        }
                    }

                    SyntaxToken name = var.Identifier;
                    AnnotationFactory.SetName(name.ValueText);
                    foreach (var qualifier in var.Qualifiers)
                    {
                        if (qualifier is RegisterLocation)
                        {
                            string register = ((RegisterLocation)qualifier).Register.ValueText;
                            if (registers.Contains(register))
                            {
                                MessageBox.Show($"Register: {register}\nDefined multiple times, ShaderBox can not decides which one needs to be binded.\n\nWill be resolved in a future release.", "Unsupported operation", MessageBoxButton.OK, MessageBoxImage.Error);
                                AnnotationFactory.DestroyGroup();
                                return false;
                            }
                            AnnotationFactory.SetRegister(register);
                        }
                    }
                    if (string.IsNullOrWhiteSpace(AnnotationFactory.GetRegister()))
                    {
                        AnnotationFactory.DestroyGroup();
                    }
                    else
                    {
                        AnnotationFactory.EndVar();
                    }

                }
            }

            return true;
        }
    }
}
