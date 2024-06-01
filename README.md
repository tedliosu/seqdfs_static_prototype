
# Instructions (Linux Version of This Program)

Build dependencies include;

1. The Boost Library Collection (MINIMUM version 1.83 required)

2. GNU Compiler Collection


Open a terminal interface and run:

- `make run` to run program
- `make run_tests` to run unit tests for SeqDFS implementation
- NOTE: if running program with input size of vertices 15000 or larger, MUST run `ulimit -s unlimited` within the terminal before running the actual program or the program will segfault due to default stack size limits; `make run` takes care of this for you :D

Note: only Linux distributions are supported for now on this main branch.

# TODO

1. Add more detailed comments

2. Add more details in this README

3. Add more unit tests

4. Add in makefile option to run program with [Microsoft mimalloc](https://github.com/microsoft/mimalloc) as memory allocator, since I've noticed improved graph reordering speeds using mimalloc to override the default memory allocator.

