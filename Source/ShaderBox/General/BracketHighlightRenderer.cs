using ICSharpCode.AvalonEdit.Document;
using ICSharpCode.AvalonEdit.Rendering;
using ShaderTools.CodeAnalysis.BraceMatching;
using System.Windows.Media;

namespace ShaderBox.General
{
    class BracketHighlightRenderer : IBackgroundRenderer
    {
        private BraceMatchingResult? _braceMatchingResult;
        private Pen _borderPen;
        private Brush _backgroundBrush;
        private TextView _textView;

        public static readonly Color DefaultBackground = Color.FromArgb(22, 0, 0, 255);
        public static readonly Color DefaultBorder = Color.FromArgb(52, 0, 0, 255);

        internal BraceMatchingResult? BraceMatchingResult
        {
            get
            {
                return _braceMatchingResult;
            }
            set
            {
                _braceMatchingResult = value;
                _textView.InvalidateLayer(Layer);
            }
        }
        public KnownLayer Layer
        {
            get
            {
                return KnownLayer.Selection;
            }
        }

        public BracketHighlightRenderer(TextView textView)
        {
            _textView = textView;
            _textView.BackgroundRenderers.Add(this);

            _borderPen = new Pen(new SolidColorBrush(DefaultBorder), 1);
            _borderPen.Freeze();

            _backgroundBrush = new SolidColorBrush(DefaultBackground);
            _backgroundBrush.Freeze();
        }

        public void Draw(TextView textView, DrawingContext drawingContext)
        {
            if (BraceMatchingResult == null)
            {
                return;
            }

            BackgroundGeometryBuilder builder = new BackgroundGeometryBuilder();
            builder.CornerRadius = 1;
            builder.AlignToWholePixels = true;

            builder.AddSegment(textView, new TextSegment() { StartOffset = BraceMatchingResult.Value.LeftSpan.Start, Length = BraceMatchingResult.Value.LeftSpan.Length });
            builder.CloseFigure(); // prevent connecting the two segments
            builder.AddSegment(textView, new TextSegment() { StartOffset = BraceMatchingResult.Value.RightSpan.Start, Length = BraceMatchingResult.Value.RightSpan.Length });

            Geometry geometry = builder.CreateGeometry();
            drawingContext.DrawGeometry(_backgroundBrush, _borderPen, geometry);
        }
    }
}
