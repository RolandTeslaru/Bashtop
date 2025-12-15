# Bashtop 2

**Cross-platform terminal system monitor written in C++.**

![Screen Recording 2025-12-10 at 09 19 02-2-2](https://github.com/user-attachments/assets/6a6e688b-5974-4dd7-9189-cdd5575c45f3)


Reads CPU usage (total + per core), memory usage (currently none), platform info (mac and partially linux) and displays them in a terminal interface.

Current Implementation Status

CPU Usage

	•	✅ macOS: using mach host API.
	•	✅ Linux: reading from /proc/stat.
	•	✅ Windows: NtQuerySystemInformation from ntdll.dll 

💾 Platform Information

	•	✅ macOS: Full implementation using unix headers + sysctl
	•	⚠️ Linux: Partial (same unix headers like on mac but not sysctl so no cpu name and other stuff).
	•	⏳ Windows: Not yet implemented.

🧮 Memory Usage

	•	🚧 nope
