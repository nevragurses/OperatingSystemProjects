PART-2:
Part-2 is working as:
gcc makeFileSystem.c -o makeFileSystem
./makeFileSystem 4 400  fileSystem.data

PART-3:
Part-3 is working as:
gcc fileSystemOper.c -o fileSystemOper
./fileSystemOper fileSystem.data operation parameters 

NOTES:
File and directory name lengths can be at most 30,otherwise I write error message and exit.
There can be at most 4 files and 3 directory in a directory.If this limit is exceed,I write error message and exit.
There is a limit for number of directory and files,if this limit is exceed,I write error message and exit.
There is a limit for data blocks.If this limit is exceed,I write error message and exit.
There is a limit for i-nodes number.If this limit is exceed,I write error message and exit.