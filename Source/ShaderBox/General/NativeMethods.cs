using System;
using System.Runtime.ExceptionServices;
using System.Runtime.InteropServices;
using System.Security;
using System.Windows;

namespace ShaderBox.General
{
    public static class NativeMethods
    {
        private static bool running = false;
        /// <summary>
        /// Variable used to track whether the missing dependency dialog has been displayed,
        /// used to prevent multiple notifications of the same failure.
        /// </summary>
        private static bool errorHasDisplayed;

        [DllImport("ShaderRuntimeCompiler.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CompileShader(string content, string target, string shaderLocation, bool saveToDisk, IntPtr callback);

        [DllImport("ShaderRuntimeCompiler.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void DeleteArray(ref IntPtr ptr);


        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Init();

        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Cleanup();

        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Render(IntPtr resourcePointer, bool isNewSurface);

        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int UpdateShaders([MarshalAs(UnmanagedType.LPWStr)]string identifier,
           [MarshalAs(UnmanagedType.LPWStr)]string vertex,
           [MarshalAs(UnmanagedType.LPWStr)]string hull,
           [MarshalAs(UnmanagedType.LPWStr)]string domain,
           [MarshalAs(UnmanagedType.LPWStr)]string geometry,
           [MarshalAs(UnmanagedType.LPWStr)]string pixel);

        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]

        public static extern int UpdateShaderImage(uint cbufferIndex, int shaderType, [MarshalAs(UnmanagedType.LPWStr)]string path);

        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int UpdateShaderVariables(uint cbufferIndex, int shaderType, byte[] data, uint length);

        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int RenderThumbnail([MarshalAs(UnmanagedType.LPWStr)]string model,
           [MarshalAs(UnmanagedType.LPWStr)]string saveLocation);

        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int RenderThumbnailActive([MarshalAs(UnmanagedType.LPWStr)]string saveLocation);

        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]

        public static extern int SetModel([MarshalAs(UnmanagedType.LPWStr)]string filePath);

        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int UpdateShadersPP([MarshalAs(UnmanagedType.LPWStr)]string identifier,
           [MarshalAs(UnmanagedType.LPWStr)]string vertex,
           [MarshalAs(UnmanagedType.LPWStr)]string pixel);

        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int SetTopology(int topology);

        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int SetRasterizerState(int cullMode, int fillMode);

        [DllImport("D3DVisualisation.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int SetCameraOffset(float offset);

        //  [DllImport("winmm.dll", EntryPoint = "mciSendStringA")]
        //  private static extern int mciSendString(string lpstrCommand, string lpstrReturnString, int uReturnLength, int hwndCallback);

        /// <summary>
        /// Method used to invoke an Action that will catch DllNotFoundExceptions and display a warning dialog.
        /// </summary>
        /// <param name="action">The Action to invoke.</param>
        [HandleProcessCorruptedStateExceptions] 
        [SecurityCritical]
        public static void InvokeWithDllProtection(Action action)
        {
            InvokeWithDllProtection(
                () =>
                {
                    action.Invoke();
                    return 0;
                });
        }

        /// <summary>
        /// Method used to invoke A Func that will catch DllNotFoundExceptions and display a warning dialog.
        /// </summary>
        /// <param name="func">The Func to invoke.</param>
        /// <returns>The return value of func, or default(T) if a DllNotFoundException was caught.</returns>
        /// <typeparam name="T">The return type of the func.</typeparam>
        [HandleProcessCorruptedStateExceptions] 
        [SecurityCritical]
        public static T InvokeWithDllProtection<T>(Func<T> func)
        {
            try
            {
                if (running)
                {
                    MessageBox.Show("StackTrace:\n" + Environment.StackTrace);
                }
                running = true;
                T t = func.Invoke();
                running = false;
                return t;
            }
            catch (DllNotFoundException)
            {
                if (!errorHasDisplayed)
                {
                    MessageBox.Show("DLL Not Found", "WPF D3D Interop", MessageBoxButton.OK, MessageBoxImage.Error);
                    errorHasDisplayed = true;

                    if (Application.Current != null)
                    {
                        Application.Current.Shutdown();
                    }
                }
            }
            catch(ExternalException e)
            {
                MessageBox.Show(e.Message + "\n\n" + e.StackTrace, "WPF D3D Interop", MessageBoxButton.OK, MessageBoxImage.Error);
            }
            catch(Exception e)
            {
                MessageBox.Show(e.Message + "\n\n" + e.StackTrace, "WPF D3D Interop", MessageBoxButton.OK, MessageBoxImage.Error);
            }
            return default(T);
        }
    }
}