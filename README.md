# Bashtop 2

**Cross-platform terminal system monitor written in C++.**

![Bashtop2 demo](https://github.com/user-attachments/assets/e4f18f53-2a06-4b68-a549-119c195cba15)

Reads CPU usage (total + per core), memory usage (currently none), platform info (mac and partially linux) and displays them in a terminal interface.

Current Implementation Status

CPU Isage

	•	✅ macOS: using mach host API.
	•	✅ Linux: reading from /proc/stat.
	•	✅ Windows: NtQuerySystemInformation from ntdll.dll 

💾 Platform Information

	•	✅ macOS: Full implementation using unix headers + sysctl
	•	⚠️ Linux: Partial (same unix headers like on mac but not sysctl so no cpu name and other stuff).
	•	⏳ Windows: Not yet implemented.

🧮 Memory Usage

	•	🚧 nope
