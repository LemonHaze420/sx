# sx
(S)ection (X)tractor aims to extract "sections" from a directory of files while respecting the original file structure. 

A "section" is defined as anything which resembles the following structure:-
		
		*---------------------------------------------------------------------------------------*
		| Offset 	| Size	| Value								|
		*---------------------------------------------------------------------------------------*
		| 0x0		|  4	| FOURCC Identifier (ie. 'PVRT' for PowerVR textures)		|
		| 0x4		|  4	| Size of section in bytes					|
		*---------------------------------------------------------------------------------------*

By default, `sx` will find and extract PowerVR textures found in any of the input files (denoted by the `PVRT` FOURCC).. however, support for other sections can easily be added by adjusting the arrays at the top of `sx.cpp`.

Files are written to the input/current directory, with their FOURCC identifiers used as the file extension, or optionally to another directory with the third argument.

Example usage:-

	* sx.exe "/path/to/files"
	* sx.exe "/path/to/files" "/path/to/output"
