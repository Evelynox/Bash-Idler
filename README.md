# BashIdler
A lightweight terminal-based idle game written in C++, designed to be simple, clean, and fun to tinker with.  
Unlock generators, automate your point production, and let the game run passively in the background while you chill.

---

## ✨ Features
- **Fully offline C++ idle experience** - No internet required, pure terminal gaming
- **Multiple unlockable generators** with scaling output and Linux-themed names
- **Dual upgrade system** - Boost both income and speed of your generators independently
- **Command system with aliases** - Rename any command to your liking (like `mv systemctl upgrade`)
- **Persistent save/load system** - JSON-based saves that remember everything
- **Customizable interface** - Toggle compact numbers, colored output, and more
- **Clean terminal interface** - Mimics actual Linux terminal with bash-style prompts
- **Multithreaded architecture** - Passive income updates in real-time
- **Cross-platform compatibility** - Runs on Linux and Windows (via g++ or MinGW)
- **Debug mode** - Hidden developer features for testing and tweaking

---

## 🎮 Game Mechanics
- **Generators**: Purchase Linux-themed generators (`systemd`, `bash`, `cron`, etc.) that produce income
- **Upgrades**: Improve each generator's income multiplier and production speed separately  
- **Scaling economy**: Costs increase exponentially, creating strategic depth
- **Passive income**: Generators work automatically while the game runs
- **Command customization**: Rename any command using the `mv` system

---

## 🖥️ Commands
The game uses Linux-style commands that you can customize:

| Default Command | Function        | Description                                 |
|----------------|------------------|---------------------------------------------|
| `yay`          | Buy Generator    | Purchase a new generator                    |
| `cat`          | Show Cost        | Display cost of next generator              |
| `systemctl     | Upgrade          | Upgrade generator N's income or speed       |
| `echo`         | Balance          | Show current money                          |
| `lsblk`        | List Generators  | Display all owned generators                |
| `ls [N]`       | Show Stats       | Show detailed stats for generator N         |
| `mv [old] [new]` | Rename Command | Rename any existing command                 |
| `clear`        | Clear Screen     | Clear terminal (with fastfetch)             |
| `man`          | Help             | Show command list                           |
| `umount`       | Save Game        | Save current progress                       |
| `mount`        | Load Game        | Load saved progress                         |
| `usermod`      | Set Username     | Change your username                        |
| `hostname`     | Change Hostname  | Change the hostname of your session         |
| `settings`     | Settings Menu    | Configure display options                   |


---

## 🔧 Installation
**Option 1: Download from Releases**  
Just grab the latest binary from the releases page.

**Option 2: Build it yourself**
```bash
git clone https://github.com/Evelynox/Bash-Idler
cd BashIdler
g++ -std=c++17 -pthread -o bashidler main.cpp engine.cpp
./bashidler
```

**Requirements:**
- C++17 compatible compiler
- pthread support
- Optional: fastfetch for enhanced clear screen

---

## 💾 Save System
- **Automatic JSON saves** - All progress stored in structured format
- **Cross-platform paths** - `~/.local/share/bashidler/` on Linux, `%APPDATA%\BashIdler\` on Windows
- **Complete persistence** - Saves generators, upgrades, settings, and custom commands
- **Human-readable** - JSON format makes saves easy to inspect or modify

---

## 🎨 Customization
- **Compact numbers** - Toggle between `1500` and `1.5k` display
- **Colored output** - Enable terminal colors for better visibility  
- **Command aliases** - Rename any command to match your workflow
- **Username** - Customize your bash prompt with `usermod [name]`

---

## 📃 License
[GNU General Public License v3.0](LICENSE.md)

---

Made with 💙 by Addiv
