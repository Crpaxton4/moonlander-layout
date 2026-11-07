#!/usr/bin/env bash
set -euo pipefail

repository="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
# shellcheck source=versions.env
. "$repository/versions.env"

: "${HID_LISTEN_VERSION:?not set or empty}"
: "${HID_LISTEN_SHA256:?not set or empty}"

# uninstall-key-log-capture.sh uses the same task name and folder
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

# Only the name of the zip file has the version, so the hash pins the zip
download_folder="$repository/.build/key_log_capture"
archive="$download_folder/hid_listen_$HID_LISTEN_VERSION.zip"
archive_url="https://www.pjrc.com/teensy/hid_listen_$HID_LISTEN_VERSION.zip"
mkdir -p "$download_folder"
curl -fsSL -o "$archive" "$archive_url"
echo "$HID_LISTEN_SHA256  $archive" | sha256sum -c -
unzip -o -q -j "$archive" hid_listen/binaries/hid_listen.exe -d "$download_folder"

# Windows cannot replace hid_listen.exe while it runs
processes="$(tasklist.exe /fi "IMAGENAME eq hid_listen.exe" /fo csv /nh)"
if [[ "$processes" == *'"hid_listen.exe"'* ]]; then
    taskkill.exe /im hid_listen.exe /f
fi

# Log files contain all typed text, so git must not see them
mkdir -p "$install_folder"
if git -C "$install_folder" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    echo "$install_folder_windows is in a git work tree" >&2
    exit 1
fi
cp "$download_folder/hid_listen.exe" \
    "$repository/src/key_log_capture/start-key-log-capture.ps1" "$install_folder/"

user_id="$(whoami.exe)"
user_id="${user_id%$'\r'}"
start_script_windows="$install_folder_windows\\start-key-log-capture.ps1"
powershell_arguments="-NoProfile -NonInteractive -ExecutionPolicy Bypass"
powershell_arguments+=" -WindowStyle Hidden -File \"$start_script_windows\""
task="$(<"$repository/src/key_log_capture/key-log-capture-task.xml")"
task="${task//@USER_ID@/"$user_id"}"
task="${task//@POWERSHELL_ARGUMENTS@/"$powershell_arguments"}"
task="${task//@INSTALL_FOLDER@/"$install_folder_windows"}"
# schtasks reads a task file only as UTF-16, so the file gets that encoding
printf '%s\n' "$task" | iconv -f UTF-8 -t UTF-16 \
    >"$install_folder/key-log-capture-task.xml"
schtasks.exe /create /tn "$task_name" /f \
    /xml "$install_folder_windows\\key-log-capture-task.xml"

# Logon trigger starts the capture only at the next logon
schtasks.exe /run /tn "$task_name"
echo "Log files go into $install_folder_windows\\logs"
