Secure Software Lab 3
Ryan Morehart

Task 1: 
	Display alphabetic characters in a file.
	
	Compile: gcc -o task1 task1.c
	Run: ./task1 <filepath>
	Capabilities: detect binary and empty files
	
	Output samples:
		Text file:
			traherom@vmramlap:~/src/afit/secure_software/lab3$ ./task1 task1.c
			########## BEGIN ##########
			HWSecureSofwareLabtaskAuthorRyanMorehartDisplaysallalphabeticcharacte
			rsinafileincludestdiohincludectypehintmainintargccharargvcharfileN
			ameFILEfilecharccharisEmptycharisBinarycharcontainsAlphaHandlecommandli
			neifargcprintfUsagesfilenargvreturnfileNameargvOpenfilefilefopenfileNamer
			iffileprintfUnabletoopensnfileNamereturnprintfBEGINnReadanddisplayisEm
			ptycontainsAlphaisBinarycfgetcfilewhilecEOFWellclearlynotemptyifwegother
			eisEmptySkipnonalphabeticcharactersifisalphaccontainsAlphaprintfccBinary
			IsitsomesortofcontrolnontextycharacterifcEOFccccisBinaryNextcfgetcfileDone
			fclosefileifcontainsAlphaprintfnprintfENDnAnyadditionalinfotooutputifisEmpt
			yprintfFileisemptynelseifisBinaryprintfFileisbinarynelseifcontainsAlphaprint
			fFilecontainsnoalphabeticcharactersnreturn
			########### END ###########
	
		Binary file:
			traherom@vmramlap:~/src/afit/secure_software/lab3$ ./task1 task1
			########## BEGIN ##########
			ELFTTTThhhDDPtdhhhQtdRtdlibldlinuxsoGNUGNUGhKICPgmonstartlibcsoIOstdinusedfope
			nputsputcharprintffgetcfclosectypebloclibcstartmainGLIBCGLIBCGLIBCiipiiziiS
			########### END ###########
			File is binary

		Empty file:
			traherom@vmramlap:~/src/afit/secure_software/lab3$ ./task1 test
			########## BEGIN ##########
			########### END ###########
			File is empty

	Task 2:
		Exploit given command-injection vulnerable program.
		
		Notes: By using a semicolon, ampersand, or other special characters, unintended
			commands can be run. To avoid having the shell grab those special characters
			(before they actually get passed to the program), double quotes are needed 
			around the argument.
		
		Output samples:
			Normal usage:
				traherom@vmramlap:~/src/afit/secure_software/lab3$ ./task2 test
				crhchrchrhc

			Command injection usage:
				traherom@vmramlap:~/src/afit/secure_software/lab3$ ./task2 "test; ls"
				crhchrchrhc
				readme.txt  task1  task1.c  task2  task2.c  task3  task3.c  test

	Task 3:
		Fix command-injection vulnerability.
		
		Compile: gcc -o task3 task3.c
		Run: ./task3 <filepath>
		
		Notes: The directions specify to filter for a semicolon. However, there are more
			dangerous characters than just that, so we also cover ampersands (&),
			backticks (`), and pipes (|). In addition, since no blacklist is perfect,
			we use the much more secure execlp() to call the program. This actually negates
			the need for the blacklist, as demonstrated in the final sample output.
			
		Output samples:
			Normal usage:
				traherom@vmramlap:~/src/afit/secure_software/lab3$ ./task3 test
				crhchrchrhc

			Exploit attempt:
				traherom@vmramlap:~/src/afit/secure_software/lab3$ ./task3 "test; ls"
				The file name you supplied appears to be invalid.
				Enter a new one (up to 100 characters): test & ls
				The file name you supplied appears to be invalid.
				Enter a new one (up to 100 characters): test ls
				cat: test ls: No such file or directory
		
			Blacklist-disabled exploit attempt:
				traherom@vmramlap:~/src/afit/se	cure_software/lab3$ ./task3 "test; ls"
				cat: test; ls: No such file or directory
				
