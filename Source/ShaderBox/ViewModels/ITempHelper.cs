using ShaderBoxBridge;

namespace ShaderBox.ViewModels
{
    interface ITempHelper
    {
        ViewportHost ViewportHost { get; }

        bool IsStandardShaderActive { get; }
    }
}
