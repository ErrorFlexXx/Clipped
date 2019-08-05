# Clipped [![Build status](https://ci.appveyor.com/api/projects/status/wfmymx0rithwj85l/branch/master?svg=true)](https://ci.appveyor.com/project/ChristianLpke/clipped/branch/master)

The basic idea behind clipped is, that many applications need exactly the same support functions to get their work done.
_Clipped_ is intended to be a multipurpose library, that delivers small libraries to support you by whatever application you write.
Clipped is structured in libraries to prevent including tons of unused code to a project, which may need only some types of clustered functionalities.

## Current features:
The following libraries exist (at the moment of writing this file):

### ClippedUtils (The only mandatory library for all other Clipped libraries)
- uLogger -- Single instance logger with different Log Levels and file logging support.
- uString -- std::string wrapper to bundle popular string manipulation functions like e.g. _split_, _indexOf_, and much more.
- uTime -- Work with times. Stores times as utc and can convert to local system time. Conversion to different data formats. toString.
- uMemory -- Converts a memory amount of bytes to human readable strings.
- uOsDetect -- Sets definitions regarding to the current operating system it is compiled on.
- uPath -- Delivers manipulation functions for strings that contain a path (like _getDirectory_, _getFilename_, _getFilenameWithExtension_).
- DataStructures -- Implements data structures like a Tree or a binary space partitioned Tree (BspTree)
- Allocators -- Implements custom allocators to control where objects are stored in the physically memory.

### ClippedEnvironment
Deals with environment variables, the executable file path and environment informations (like amount of ram or cpus).

### ClippedFilesystem
File -- Basic functions to get informations about a file or move, copy or delete it.
BinFile -- A binary file writer.
Archives -- Implementations of archive file reading / writing.

### ClippedDataStreams
Implementations to store or read data from storage containers.

### ClippedMaths
Delivers tools to work with Vectors(2d, 3d, and 4d), Matrixes (4d) and Quaternions.
