using System.Diagnostics;
using System.IO;
using System.Text.Json;
using System.Windows;
using Microsoft.Win32;

namespace GlobalMP.Launcher;

public partial class MainWindow : Window
{
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