# Shell

This is a simple command line interpreter written in C. It supports basic command line operations such as executing external programs, piping, and input/output redirection.
Getting Started

To use the command line interpreter, simply clone this repository and run the make command to build the program. Once the program has been built, run the executable cli and start entering commands.

# shell

$ git clone https://github.com/username/cli.git
$ cd cli
$ make
$ ./cli

# Usage

The command line interpreter supports the following operations:
Executing external programs

To execute an external program, simply type the name of the program followed by any necessary arguments. For example:

shell

$ ls -al

# Piping

To pipe the output of one program into the input of another program, use the | symbol. For example:

shell

$ ls | grep file

Input/output redirection

To redirect input or output from/to a file, use the < or > symbols. For example:

lua

$ sort < input.txt 

# Concurrent execution

To execute a command in the background, add an & symbol at the end of the command. For example:

shell

$ sleep 10
