# SimpleC Compiler

This project includes a custom compiler for a simplified C-like language called SimpleC.
The compiler will translate a source language to a target language. Source language will
be SimpleC and the target language is (IR) of the LLVM compiler framework that backs
the `clang` C/C++ compiler, and many more.

Compiler will cover lexical specification on:
  - Arithmetic
  - Expressions
  - Variables
  - Control Flow
  - Function Abstraction

## Guide to the Command Line Installing the required packages
This was tested on Windows Subsystem Linux Ubuntu 18.04.

packages required:

sudo apt-get update
sudo apt-get install clang llvm make python3 python3-distutils

## Setting up Makefile
upon running `make` on the command line at the root file it will build
a .0, simplec file and link the .c file to build upon.

## Running your compiler
upon any change to original .c file, run `make` to recompile it.

steps to run project after building files from command `make`:

  Take the `example1.simplec` test file and apply it to a resulting LLVM IR file.

    ./simplec testcases/example1.simplec > program.ll

  The LLVM IR output will be converted to machine code to run the output program.

    clang -o program program.ll
    ./program

## Source
testcases, makefile, template.h file provided by professor Paul Gazzillo (https://paulgazzillo.com)
