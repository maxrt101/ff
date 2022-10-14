<p align="center">
  <img src="https://github.com/maxrt101/ff/blob/main/docs/logo.png?raw=true" width="300">
</p>

<h1 align="center">FF Programing Language</h1>

<!--
[![build](https://github.com/maxrt101/ff/actions/workflows/build.yml/badge.svg)](https://github.com/maxrt101/ff/actions/workflows/build.yml)
-->

<p align="center">
  <a href="https://github.com/maxrt101/ff/actions/workflows/build.yml">
    <img src="https://github.com/maxrt101/ff/actions/workflows/build.yml/badge.svg" alt="build"> 
  </a>
</p>

## About
ff is a general purpose scripting language with dynamic and gradual typing.  
It was developed to be easily used within C++ applications (which also means bindings for other languages should be fairly easy to implement).  


## Building
### Prerequisites:
  - Linux or MacOS environment (WSL works too)
  - `C++` compiler that supports `c++17` (tested with `apple clang 11.0.3`, `gcc 10.2.0` and `gcc 9.4.0`)
  - `python3` (tested with `3.10.0`)

### Steps:
 - Clone the repo
 - `cd ff`
 - `git submodule update --init --recursive`
 - `./make.py`

Executable will appear in `target/release/bin`, headers and libraries in `target/release/include` and `target/release/lib` respectively.  

For more verbose output use `-v`  
To see output of the commands, use `-o`  

To make a debug build, use `-p debug` option, you can also provide a (comma separated) list of features with `--feature FEATURES`.  
Supported features: `LOG_STDOUT_ONLY`.  
Supported debug features: `MEM`, `REF`, `EVAL`, `DISASM`, `TOKENS`, `TREE`, `TRACE`, `SCOPES`, `GLOBALS`, `NOCATCH`.  

Build system keeps track of changed source files, and on subsequent builds will only recompile files that have changed. To force recompilation of everything, use `-f` flag.  

### Tests:
To run tests execute `./make.py test` (or directly with `./tests/run.sh`)  
Usage of `run.sh`: `./tests/run.py [OPTION] PROFILE [TEST...]`  
