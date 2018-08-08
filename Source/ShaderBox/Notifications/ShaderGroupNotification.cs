using Prism.Interactivity.InteractionRequest;
using ShaderBox.Models;

namespace ShaderBox.Notifications
{
    public class ShaderGroupNotification : Confirmation
    {
        public ShaderGroup ShaderGroup { get; set; } = new ShaderGroup("", ShaderGroupType.Standard);
    }
}
