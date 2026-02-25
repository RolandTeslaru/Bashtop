# Bashtop 2

**Cross-platform terminal system monitor written in C++.**

![ScreenRecording2026-02-25at11 16 16-ezgif com-optimize](https://github.com/user-attachments/assets/6272c6e3-bae4-4b57-a49d-ee31ba73bef3)

Reads CPU usage (total + per core), memory usage, platform info and displays them in a terminal interface.

Current Implementation Status

CPU Usage

	•	✅ macOS: using mach host API.
	•	✅ Linux: reading from /proc/stat.
	•	✅ Windows: NtQuerySystemInformation from ntdll.dll 

Platform Information

	•	✅ macOS: sysctlbyname + uname.
	•	⚠️ Linux: uname + sysconf. Missing: cpu name, os build, model id.
	•	⏳ Windows: Not yet implemented.

Memory Usage

	•	✅ macOS: sysctl + host_statistics64 (Mach API).
	•	✅ Linux: reading from /proc/meminfo.
	•	✅ Windows: using GlobalMemoryStatusEx api.
