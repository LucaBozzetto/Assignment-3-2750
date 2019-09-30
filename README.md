# VCard Parser - CIS*2750 Assignment 3

![App screenshot](https://user-images.githubusercontent.com/2963072/65902887-a6e62180-e3bb-11e9-9be7-d152f3cdccde.png)

This is a partial implentation of a VCard Parser.  
The parser is only working on a specific set of features (eg Geographical properties are not validated).  
The vCard format specification can be found [here](https://tools.ietf.org/html/rfc6350).  
The project is split in two main section: 
  1. The actual vCard parser written in C
  2. A web GUI that runs on NodeJS and uses the parser library
  
Through the GUI users can upload, download and inspect vCard files after they are parsed and validated.  
The assignment's focus was to implement a standard and apply defensive programming techniques.  
The C code was extensively tested for memory leaks using [Valgrind](http://valgrind.org/).  
Moreover a test harness suite developed by UoG TAs was run in order to check the code for errors.  
At the moment the project is meant to be used only on Linux or MacOS.  

## Installation and Running

### 1. Compile Library

```Bash
# From /parser directory
make
```

### 2. Install

```Bash
# From the root of the directory
npm install
```

### 3. Running Server

```Bash
npm run dev PORT
# Server will be accessible at http://localhost:PORT
```

