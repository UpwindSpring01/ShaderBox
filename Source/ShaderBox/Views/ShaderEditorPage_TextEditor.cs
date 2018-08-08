using ShaderBox.General;
using ShaderTools.CodeAnalysis.Hlsl.BraceMatching;
using ShaderTools.CodeAnalysis.Hlsl.Syntax;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Windows.Input;
using ShaderTools.CodeAnalysis.Text;
using ShaderTools.CodeAnalysis.Hlsl.Formatting;
using ShaderTools.CodeAnalysis.BraceMatching;
using FirstFloor.ModernUI.Windows.Controls;

namespace ShaderBox.Views
{
    public partial class ShaderEditorPage : ModernFrame
    {
        private BracketHighlightRenderer _bracketHighlightRenderer = null;
        private HlslBraceMatcher _braceMatcher;

        private void InitializeTextEditor()
        {
            textEditor.TextArea.PreviewKeyDown += TextEditor_TextArea_PreviewKeyDown;
            textEditor.TextArea.Caret.PositionChanged += TextEditor_TextArea_Caret_PositionChanged;

            _bracketHighlightRenderer = new BracketHighlightRenderer(textEditor.TextArea.TextView);
            textEditor.TextArea.Options.HighlightCurrentLine = true;

            _braceMatcher = new HlslBraceMatcher();
        }

        private void TextEditor_TextArea_Caret_PositionChanged(object sender, EventArgs e)
        {
            // Should only reparse when text changed
            SyntaxTree syntaxTree = SyntaxFactory.ParseSyntaxTree(SourceText.From(textEditor.Text), null);
            BraceMatchingResult? result = _braceMatcher.FindBraces(syntaxTree, syntaxTree.MapRootFilePosition(textEditor.CaretOffset), new CancellationToken());
            _bracketHighlightRenderer.BraceMatchingResult = result;
        }

        private void TextEditor_TextArea_TextEntered(object sender, TextCompositionEventArgs e)
        {
            
        }

        private void TextEditor_TextArea_TextEntering(object sender, TextCompositionEventArgs e)
        {
        }

        void TextEditor_TextArea_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.F && 
                (Keyboard.Modifiers == (ModifierKeys.Control | ModifierKeys.Shift)))
            {
                e.Handled = true;
                textEditor.Text = Format(textEditor.Text);
            }
        }

        private string Format(string unformattedText, FormattingOptions options = null)
        {
            SyntaxTree syntaxTree = SyntaxFactory.ParseSyntaxTree(SourceText.From(unformattedText), null);
            TextSpan span = new TextSpan(0, unformattedText.Length);
            if (options == null)
                options = new FormattingOptions();

            IList <TextChange> edits = Formatter.GetEdits(syntaxTree, (SyntaxNode)syntaxTree.Root, span, options);
            return Formatter.ApplyEdits(unformattedText, edits);
        }
    }
}
