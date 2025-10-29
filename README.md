# Bashtop 2

**Cross-platform terminal system monitor written in C++.**

Reads CPU (currently mac and linux), memory sensors (currently none), platform info (mac and partially linux) and displays them in a terminal interface (not yet).

<img width="227" height="253" alt="Screenshot 2025-10-29 at 02 18 04" src="https://github.com/user-attachments/assets/4692729c-eb3f-4935-a8fc-65ed74a01a58" />
<img width="302" height="190" alt="Screenshot 2025-10-29 at 02 34 21" src="https://github.com/user-attachments/assets/89ff6d9f-21fc-467e-b8ad-06bf1cd666c5" />

Current Implementation Status

CPU Monitoring

	•	✅ macOS: using mach host API.
	•	✅ Linux: reading from /proc/stat.
	•	⏳ Windows: Not yet implemented.

💾 Platform Information

	•	✅ macOS: Full implementation using unix headers + sysctl
	•	⚠️ Linux: Partial (same unix headers like on mac but not sysctl so no cpu name and other stuff).
	•	⏳ Windows: Not yet implemented.

🧮 Memory Monitoring

	•	🚧 nope
