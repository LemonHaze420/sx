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

## Configuration

Additionally, sections can be added to sx by making use of `sx.ini` (placed in the same directory as the executable). 

The below examples disables the built-in `PVRT` section and enables it through the configuration, for demonstration purposes. Section names are limited to 4 bytes/characters and must be ASCII (for now).

	[general]
	version=1
	disable_pvrt=true
	exclude_extensions=.XYZ,.ZZZ

	[sections]
	SectionName0=PVRT
	SectionSize0=0

Example usage:-

	* sx.exe "/path/to/files"
	* sx.exe "/path/to/files" "/path/to/output"
