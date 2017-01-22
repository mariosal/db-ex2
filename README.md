# K18 - [Project 2](docs/Assignment.pdf)

## Authors

- Mario Saldinger <sdi1400177@di.uoa.gr>
- Nikolaos Iosifidis <sdi1400051@di.uoa.gr>

## Requirements

- [GNU Compiler Collection](https://gcc.gnu.org/)
- [GNU Make](https://gnu.org/software/make/)

## Compilation

```sh
make
```

## Execution

```sh
./build/sorted.out < etc/dataset/10000.csv  # [Default: -fieldno 0]
./build/sorted.out -value 11903588 < etc/dataset/10000.csv
./build/sorted.out -fieldno 1 < etc/dataset/10000.csv
./build/sorted.out -fieldno 3 -value Piraeus < etc/dataset/10000.csv
./build/sorted.out -filename build/heap_file -fieldno 3 -value Piraeus < etc/dataset/10000.csv
```

## Cleaning

```sh
make clean
```

## Implementation

Everything has been implemented.

### Operating systems

- [Fedora](https://getfedora.org/) 25
- [Ubuntu](https://www.ubuntu.com/) 12.04.5 LTS

### Tools

- [Clang](http://clang.llvm.org/) 3.8.0
- Git 2.9.3
- GNU Compiler Collection 6.3.1
- GNU Make 4.1
- [Valgrind](http://valgrind.org/) 3.12.0
