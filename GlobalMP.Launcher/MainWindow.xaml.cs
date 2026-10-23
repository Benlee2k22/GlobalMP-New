using System.Diagnostics;
using System.IO;
using System.Text.Json;
using System.Windows;
using Microsoft.Win32;

namespace GlobalMP.Launcher;

public partial class MainWindow : Window
{
    private Process? _clientProcess;
    private bool _serverConfirmed;

    private static readonly string SettingsFile = Path.Combine(
        Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
        "GlobalMP", "launcher.json");

    public MainWindow()
    {
        InitializeComponent();
        try
        {
            if (File.Exists(SettingsFile))
            {
                var settings = JsonSerializer.Deserialize<LauncherSettings>(File.ReadAllText(SettingsFile));
                GamePathBox.Text = settings?.GamePath ?? "";
            }
        }
        catch (Exception ex)
        {
            StatusText.Text = $"Unable to load settings: {ex.Message}";
        }
    }

    private void Browse_Click(object sender, RoutedEventArgs e)
    {
        var dialog = new OpenFileDialog
        {
            Title = "Select Euro Truck Simulator 2",
            Filter = "ETS2 executable (eurotrucks2.exe)|eurotrucks2.exe",
            CheckFileExists = true
        };
        if (dialog.ShowDialog() == true)
        {
            GamePathBox.Text = dialog.FileName;
            SaveSettings();
            StatusText.Text = "ETS2 executable selected.";
        }
    }

    private void Launch_Click(object sender, RoutedEventArgs e)
    {
        var path = GamePathBox.Text.Trim().Trim('"');
        if (!File.Exists(path) || !string.Equals(Path.GetFileName(path), "eurotrucks2.exe", StringComparison.OrdinalIgnoreCase))
        {
            StatusText.Text = "Choose a valid eurotrucks2.exe first.";
            return;
        }
        try
        {
            Process.Start(new ProcessStartInfo(path)
            {
                WorkingDirectory = Path.GetDirectoryName(path)!,
                UseShellExecute = true
            });
            SaveSettings();
            StatusText.Text = "ETS2 launched. Multiplayer game integration is not installed yet.";
        }
        catch (Exception ex)
        {
            StatusText.Text = $"Launch failed: {ex.Message}";
        }
    }


    private async void Connect_Click(object sender, RoutedEventArgs e)
    {
        if (_clientProcess is { HasExited: false })
        {
            _clientProcess.Kill();
            ConnectButton.Content = "CONNECT TO GLOBALMP SERVER";
            StatusText.Text = "Disconnected by launcher.";
            return;
        }

        var exe = Path.Combine(AppContext.BaseDirectory, "globalmp-client.exe");
        if (!File.Exists(exe))
        {
            StatusText.Text = "Missing globalmp-client.exe. Download the Windows client build from GitHub Actions and place it beside the launcher executable.";
            return;
        }

        _serverConfirmed = false;
        ConnectButton.IsEnabled = false;
        StatusText.Text = "Connecting to GlobalMP server...";
        try
        {
            var process = new Process
            {
                StartInfo = new ProcessStartInfo(exe)
                {
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true
                },
                EnableRaisingEvents = true
            };
            _clientProcess = process;
            process.Start();
            ConnectButton.Content = "DISCONNECT";
            ConnectButton.IsEnabled = true;
            _ = Task.Run(async () =>
            {
                try
                {
                    while (await process.StandardOutput.ReadLineAsync() is { } line)
                    {
                        await Dispatcher.InvokeAsync(() =>
                        {
                            if (!ReferenceEquals(_clientProcess, process)) return;
                            if (line.StartsWith("JOIN_CONFIRMED"))
                            {
                                _serverConfirmed = true;
                                StatusText.Text = "Connected and accepted by GlobalMP server. " + line;
                            }
                            else if (!_serverConfirmed || line.StartsWith("ERROR") || line == "DISCONNECTED")
                                StatusText.Text = line;
                        });
                    }
                    await process.WaitForExitAsync();
                    await Dispatcher.InvokeAsync(() =>
                    {
                        if (!ReferenceEquals(_clientProcess, process)) return;
                        ConnectButton.Content = "CONNECT TO GLOBALMP SERVER";
                        ConnectButton.IsEnabled = true;
                        if (_serverConfirmed)
                            StatusText.Text = "GlobalMP connection closed.";
                        else
                            StatusText.Text = "Connection ended without server confirmation. Check VPS firewall and client build.";
                    });
                }
                catch (Exception ex)
                {
                    await Dispatcher.InvokeAsync(() => StatusText.Text = "Client error: " + ex.Message);
                }
                finally { process.Dispose(); }
            });
        }
        catch (Exception ex)
        {
            ConnectButton.IsEnabled = true;
            ConnectButton.Content = "CONNECT TO GLOBALMP SERVER";
            StatusText.Text = "Unable to start networking client: " + ex.Message;
        }
    }

    private void SaveSettings()
    {
        try
        {
            Directory.CreateDirectory(Path.GetDirectoryName(SettingsFile)!);
            File.WriteAllText(SettingsFile, JsonSerializer.Serialize(new LauncherSettings(GamePathBox.Text)));
        }
        catch (Exception ex)
        {
            StatusText.Text = $"Could not save settings: {ex.Message}";
        }
    }

    private sealed record LauncherSettings(string GamePath);
}