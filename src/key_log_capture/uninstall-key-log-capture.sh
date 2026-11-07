#!/usr/bin/env bash
set -euo pipefail

# install-key-log-capture.sh uses the same task name and folder
task_name="moonlander-key-log-capture"
# cmd.exe warns about a Linux working folder, so it starts on drive C
local_app_data="$(cd "$(wslpath -u C:/)" && cmd.exe /d /c 'echo %LOCALAPPDATA%')"
local_app_data="${local_app_data%$'\r'}"
if [[ ! "$local_app_data" =~ ^[A-Za-z]:\\ ]]; then
    echo "LOCALAPPDATA is not a Windows path: $local_app_data" >&2
    exit 1
fi
install_folder_windows="$local_app_data\\moonlander-key-log"
install_folder="$(wslpath -u "$install_folder_windows")"

if schtasks.exe /query /tn "$task_name" >/dev/null 2>&1; then
    schtasks.exe /delete /tn "$task_name" /f
fi

processes="$(tasklist.exe /fi "IMAGENAME eq hid_listen.exe" /fo csv /nh)"
if [[ "$processes" == *'"hid_listen.exe"'* ]]; then
    taskkill.exe /im hid_listen.exe /f
fi

# Only the operator decides when to delete the typed text in the log files
rm -f "$install_folder/hid_listen.exe" \
    "$install_folder/start-key-log-capture.ps1" \
    "$install_folder/key-log-capture-task.xml"
echo "Log files stay in $install_folder_windows\\logs"
