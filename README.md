
# Instructions (Linux Version of This Program)

Build dependencies include:

1. The Boost Library Collection (MINIMUM version 1.83 required)

2. GNU Compiler Collection

3. CMake minimum version 3.18 (ONLY if you want to run the program with Microsoft mimalloc as memory allocator)


Open a terminal interface and run:

- `make run` to run program
- `make run_mimalloc` to run program with [Microsoft mimalloc](https://github.com/microsoft/mimalloc) as memory allocator
- `make run_tests` to run unit tests for SeqDFS implementation
- NOTE: if running program with input size of vertices 15000 or larger, MUST run `ulimit -s unlimited` within the terminal before running the actual program or the program will segfault due to default stack size limits; `make run` takes care of this for you :D

Note: only Linux distributions are supported for now on this main branch.

# TODO

1. Add more detailed comments

2. Add more details in this README

3. Add more unit tests

4. <s>Add in makefile option to run program with [Microsoft mimalloc](https://github.com/microsoft/mimalloc) as memory allocator, since I've noticed improved graph reordering speeds using mimalloc to override the default memory allocator.</s> Done on 6/1/24 :)

# Acknowledgments

- Research paper this program was based on:
    - Qiuyi Lyu, Mo Sha, Bin Gong, and Kuangda Lyu. 2021. Accelerating Depth-First Traversal by Graph Ordering. In Proceedings of the 33rd International Conference on Scientific and Statistical Database Management (SSDBM '21). Association for Computing Machinery, New York, NY, USA, 13–24. [https://doi.org/10.1145/3468791.3468796](https://doi.org/10.1145/3468791.3468796)
- Special thanks to an experienced contributor from the official *Together C & C++* Discord for helping me understand the adjacency-list structure described in the original paper.

