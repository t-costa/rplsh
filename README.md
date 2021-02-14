# RPL-Shell
The RPL-Shell is a DSL-based toolchain supporting the design of parallel applications where parallelism is structured via proper composition of algorithmic skeletons.

## Installation
**Requirements**
- cmake
- libreadline-dev
```
git clone https://github.com/t-costa/rplsh.git
cd rplsh
./install.sh
```

## Run
After the installation, you can find the executable in `build` directory.
```
./build/rplsh
```

## Usage example
```
a = seq(10)
b = seq(20)
c = seq(50)
p = pipe(a,b,c)
rewrite p with allrules, allrules
optimize p with farmopt, pipeopt, maxresources
show p by servicetime, resources +5
```
