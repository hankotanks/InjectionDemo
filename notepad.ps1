$process = wmic process call create "notepad"

$id = $process `
    | Select-String -Pattern "ProcessId\s+=\s+(\d+)" `
    | ForEach-Object { $_.Matches.Groups[1].Value }

Start-Process -FilePath "hider.exe" -ArgumentList $id -WindowStyle Hidden

$process = Get-Process -Id $id

$windowHandle = $process.MainWindowHandle

Add-Type @"
    using System;
    using System.Runtime.InteropServices;

    public class WindowHelper {
        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool SetForegroundWindow(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern IntPtr GetForegroundWindow();
    }
"@

[WindowHelper]::SetForegroundWindow($windowHandle)