# sx
(S)ection (X)tractor aims to extract "sections" from a directory of files while respecting the original file structure. 

A "section" is defined as anything which resembles the following structure:-
		
		*---------------------------------------------------------------------------------------*
		| Offset 	| Size	| Value								|
		*---------------------------------------------------------------------------------------*
		| 0x0		|  4	| FOURCC Identifier (ie. 'PVRT' for PowerVR textures)		|
		| 0x4		|  4	| Size of section in bytes					|
		*---------------------------------------------------------------------------------------*
		
Files are written to the input/current directory, or optionally to another directory with the third argument.

Example usage:-

	* sx.exe "/path/to/files"
	* sx.exe "/path/to/files" "/path/to/output"
