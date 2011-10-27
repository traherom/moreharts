Secure Software - Lab 4
Ryan Morehart

Task 1
	Attached is a script that actually performs the exploit against
	the vulnerable program (exploit.sh). It actually operates against
	a slightly modified version of the vulnerable code (attached: vulp.c),
	as it appears that /tmp/ has special settings applied to it (the
	sticky bit?) that prevent fopen from opening a file through a symlink
	located in it. Instead, we use a symlink located in the same directory.
	
	In addition, the artificial loop delay was replaced with a call to
	sleep(), just to make the interval more reliable. In the real world
	having to hit a much shorter and less predicatable interval would
	just require multiple tries and/or forcing context switches away from
	the vulnerable program at the right moment.
	
	No other changes in the vulp program have any effect on its basic
	operation or exploitability.
	
	Although the exploit is documented in the comments of the script,
	the basic idea is this:
		1. Create XYZ as a normal file, writable by the user
		2. Start vulp and allow it to check the file is safe via access()
		3. Between that check and the file opening, change the file into
			a link that points to a root-writable file
		4. When vulp writes, it will append to the end of this file
		
	In our example, a trivial call to whoami is stuck at the end of our
	"critical file," which we assume to be some sort of script that
	root executes. In a real situtation we might use our ability to append
	to a file to add a new user to /etc/passwd and /etc/shadow; modify config
	files (often later settings override earlier ones, so security might be
	relaxed for remotely-accessible services); or make a backdoor (IE, netcat)
	run from script that will run as a root, such as an init script or cronjob.
	
Task 2
	vulp-modified.c contains one example change. Immediately after opening the
	file we re-check the access on it. If an attacker wanted to execute the same
	attack, they would need to swap out the file for a link during the first delay,
	then swap it back as soon as fopen() completes. Nailing both race conditions
	would be very difficult.
	
