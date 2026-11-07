$ErrorActionPreference = 'Stop'

$logFolder = Join-Path $PSScriptRoot 'logs'
New-Item -ItemType Directory -Force -Path $logFolder | Out-Null

# Invariant culture gives the same digits and calendar on each Windows locale
function Get-LogPath {
    $culture = [Globalization.CultureInfo]::InvariantCulture
    $startTime = (Get-Date).ToString("yyyy-MM-dd'T'HHmm", $culture)
    Join-Path $logFolder "keylog-$startTime.log"
}

# A second start in the same minute waits for the next minute, so it does not
# overwrite the file of the first start
$logPath = Get-LogPath
while (Test-Path -LiteralPath $logPath) {
    Start-Sleep -Seconds 5
    $logPath = Get-LogPath
}

# hid_listen writes into the file handle directly, thus PowerShell does not encode
# the bytes again. hid_listen waits for the keyboard and connects again after each
# disconnect, so it does not stop. hid_listen gets its own hidden console, so the
# PowerShell console can close
$hidListenStart = @{
    FilePath = (Join-Path $PSScriptRoot 'hid_listen.exe')
    RedirectStandardOutput = $logPath
    WorkingDirectory = $logFolder
    WindowStyle = 'Hidden'
}
Start-Process @hidListenStart
