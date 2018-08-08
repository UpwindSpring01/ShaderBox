using ShaderBox.Models;
using ShaderBox.Models.Annotation;
using ShaderTools.CodeAnalysis.Syntax;
using ShaderTools.CodeAnalysis.Hlsl.Syntax;
using System;
using System.Collections.Generic;
using System.Linq;

namespace ShaderBox.General
{
    public enum DataType
    {
        Bool,
        Int,
        Uint,
        Min16Int,
        Min12Int,
        Min16Uint,
        Half,
        Float,
        Double,
        Min16Float,
        Min10Float,
        Texture,
        Unsupported,
    }

    static class AnnotationFactory
    {
        private static AnnotationShaderGroup _activeShaderGroup = null;
        private static AnnotationGroup _activeGroup = null;
        private static AnnotationVariable _activeVar = null;
        private static bool _varValid = false;

        private static uint _curOffset;

        public static void Start(ShaderType shaderType)
        {
            _activeShaderGroup = new AnnotationShaderGroup();
            _activeShaderGroup.Type = shaderType;
        }
        public static void BeginGroup()
        {
            _activeGroup = new AnnotationGroup();
            _curOffset = 0;
        }

        public static void SetRegister(string valueText)
        {
            _activeGroup.Register = valueText;
        }

        public static string GetRegister()
        {
            return _activeGroup.Register;
        }

        public static void BeginVar()
        {
            _activeVar = new AnnotationVariable();
            _varValid = true;
        }

        public static void SetModifiers(List<SyntaxToken> modifiers)
        {
            // Can not access static const variables so do not add them.
            if (modifiers.Count((st) => st.Kind == SyntaxKind.StaticKeyword) == 1)
            {
                DestroyGroup();
                return;
            }

            if (modifiers == null && modifiers.Count == 0)
            {
                return;
            }
            if (!(_activeVar.IsSNorm = (modifiers.Count((st) => st.Kind == SyntaxKind.SNormKeyword) == 1)))
            {
                _activeVar.IsUNorm = (modifiers.Count((st) => st.Kind == SyntaxKind.UNormKeyword) == 1);
            }
        }

        public static void SetName(string name)
        {
            _activeVar.Name = name;
        }

        public static bool IsGroupOpen()
        {
            return _activeGroup != null;
        }

        public static void SetType(TypeSyntax typeSyntax)
        {
            if (!IsGroupOpen())
            {
                return;
            }

            if (typeSyntax.Kind == SyntaxKind.PredefinedScalarType)
            {
                _activeVar.IsScalar = true;

                ScalarTypeSyntax stSyntax = (ScalarTypeSyntax)typeSyntax;
                if (stSyntax.TypeTokens.Count == 2)
                {
                    _activeVar.DataType = GetDataType(stSyntax.TypeTokens[1].ValueText);
                    if (_activeVar.DataType == DataType.Int && stSyntax.TypeTokens[0].ValueText.Equals("unsigned"))
                    {
                        _activeVar.DataType = DataType.Uint;
                    }
                }
                else if (stSyntax.TypeTokens.Count == 1)
                {
                    _activeVar.DataType = GetDataType(stSyntax.TypeTokens[0].ValueText);
                }
                else
                {
                    _varValid = false;
                }
            }
            else if (typeSyntax.Kind == SyntaxKind.PredefinedVectorType)
            {
                _activeVar.IsVector = true;

                VectorTypeSyntax vtSyntax = (VectorTypeSyntax)typeSyntax;
                string syntaxToken = vtSyntax.TypeToken.ValueText;
                _activeVar.DataType = GetDataType(syntaxToken);
                ParseVectorColumns(syntaxToken);
            }
            else if (typeSyntax.Kind == SyntaxKind.PredefinedMatrixType)
            {
                _activeVar.IsMatrix = true;

                MatrixTypeSyntax mtSyntax = (MatrixTypeSyntax)typeSyntax;
                string syntaxToken = mtSyntax.TypeToken.ValueText;
                _activeVar.DataType = GetDataType(syntaxToken);
                ParseMatrixDimensions(syntaxToken);
            }
            else if (typeSyntax.Kind == SyntaxKind.PredefinedObjectType)
            {
                PredefinedObjectTypeSyntax otSyntax = (PredefinedObjectTypeSyntax)typeSyntax;
                if (otSyntax.ObjectTypeToken.Kind == SyntaxKind.Texture2DKeyword)
                {
                    _activeVar.IsTexture = true;
                    _activeVar.DataType = DataType.Texture;
                }
                else
                {
                    _varValid = false;
                }
            }
            else
            {
                _varValid = false;
            }
        }

        public static void SetDimension(int dimension)
        {
            _activeVar.Dimension = dimension;
        }

        private static void ParseVectorColumns(string type)
        {
            _activeVar.Rows = 1;
            _activeVar.Columns = (uint)char.GetNumericValue(type.Last());
        }

        private static void ParseMatrixDimensions(string type)
        {
            int length = type.Length;
            _activeVar.Columns = (uint)char.GetNumericValue(type[length - 1]);
            _activeVar.Rows = (uint)char.GetNumericValue(type[length - 3]);
        }

        private static DataType GetDataType(string type)
        {
            if (type.StartsWith("bool"))
            {
                return DataType.Bool;
            }
            else if (type.StartsWith("int"))
            {
                return DataType.Int;
            }
            else if (type.StartsWith("uint"))
            {
                return DataType.Uint;
            }
            else if (type.StartsWith("min16int"))
            {
                return DataType.Min16Int;
            }
            else if (type.StartsWith("min12int"))
            {
                return DataType.Min12Int;
            }
            else if (type.StartsWith("min16uint"))
            {
                return DataType.Min16Uint;
            }
            else if (type.StartsWith("half"))
            {
                return DataType.Half;
            }
            else if (type.StartsWith("float"))
            {
                return DataType.Float;
            }
            else if (type.StartsWith("double"))
            {
                return DataType.Double;
            }
            else if (type.StartsWith("min16float"))
            {
                return DataType.Min16Float;
            }
            else if (type.StartsWith("min10float"))
            {
                return DataType.Min10Float;
            }
            _varValid = false;
            return DataType.Unsupported;
        }

        public static void SetAnnotations(List<VariableDeclarationStatementSyntax> annotations)
        {
            if (annotations == null || !_varValid)
            {
                return;
            }
            _activeVar.Annotations = new Dictionary<string, string>();
            foreach (VariableDeclarationStatementSyntax vdsSyntax in annotations)
            {
                SeparatedSyntaxList<VariableDeclaratorSyntax> sSyntaxList = vdsSyntax.Declaration.Variables;
                if (sSyntaxList.Count != 1)
                {
                    _varValid = false;
                    return;
                }
                VariableDeclaratorSyntax vdSyntax = sSyntaxList[0];
                string key = vdSyntax.Identifier.ValueText;
                EqualsValueClauseSyntax evcSyntax = (EqualsValueClauseSyntax)vdSyntax.Initializer;

                string value = "";
                if (evcSyntax.Value is StringLiteralExpressionSyntax)
                {
                    StringLiteralExpressionSyntax sleSyntax = (StringLiteralExpressionSyntax)evcSyntax.Value;
                    List<SyntaxToken> tokens = sleSyntax.Tokens;
                    if (tokens.Count > 0)
                    {
                        value = tokens[0].ValueText;
                    }
                }
                else if(evcSyntax.Value is PrefixUnaryExpressionSyntax)
                {
                    PrefixUnaryExpressionSyntax ueSyntax = (PrefixUnaryExpressionSyntax)evcSyntax.Value;
                    value = ueSyntax.ToString();
                } else if(evcSyntax.Value is LiteralExpressionSyntax)
                {
                    LiteralExpressionSyntax ueSyntax = (LiteralExpressionSyntax)evcSyntax.Value;
                    value = ueSyntax.ToString();
                }
                else
                {
                    throw new Exception("Unsupported annontation");
                }

                _activeVar.Annotations.Add(key, value);
            }
        }

        public static void EndVar()
        {
            if (_varValid)
            {
                if (_activeGroup.AnnotationVariables.Count > 0 && !_activeVar.IsTexture &&
                    (
                    (_activeGroup.AnnotationVariables.Last().DataType.IsMinimum() && !_activeVar.DataType.IsMinimum()) ||
                    (!_activeGroup.AnnotationVariables.Last().DataType.IsMinimum() && _activeVar.DataType.IsMinimum())))

                {
                    _curOffset = _curOffset.RoundUpMultiple16();
                }

                _activeGroup.AnnotationVariables.Add(_activeVar);

                if (!_activeVar.IsTexture)
                {
                    uint size = 0;
                    if (_activeVar.IsScalar)
                    {
                        size = _activeVar.DataType.GetPackSizeScalar();
                    }
                    else if (_activeVar.IsVector)
                    {
                        size = _activeVar.DataType.GetPackSizeScalar() * _activeVar.Columns;
                    }
                    else if (_activeVar.IsMatrix)
                    {
                        size = (_activeVar.Columns - 1) * 16 + (_activeVar.DataType.GetPackSizeScalar() * _activeVar.Rows);
                    }
                    if(_activeVar.Dimension > 0)
                    {
                        size = size.RoundUpMultiple16();
                        size *= (uint)_activeVar.Dimension;
                    }

                    uint nextMultiple = _curOffset.RoundUpMultiple16();
                    if (_curOffset + size > nextMultiple && _curOffset > 0)
                    {
                        _curOffset = nextMultiple;
                    }
                    _activeVar.Offset = _curOffset;
                    _activeVar.Size = size;
                    _curOffset += size;
                }
            }
            _activeVar = null;
        }

        public static void EndGroup()
        {
            if (_activeGroup != null)
            {
                _activeShaderGroup.Buffers.Add(_activeGroup);

                // Images do not use pack
                _activeGroup.BufferSize = _curOffset.RoundUpMultiple16();
                _activeGroup = null;
            }
        }

        public static void DestroyGroup()
        {
            _activeVar = null;
            _activeGroup = null;
        }

        public static AnnotationShaderGroup End()
        {
            AnnotationShaderGroup group = _activeShaderGroup;
            _activeShaderGroup.CreateUI();
            _activeShaderGroup = null;
            return group;
        }
    }
}
