
.PHONY: all tests clean

MAIN_SRC_FILES := seqdfs_vs_boost_dfs_main.cpp seqdfs_vs_boost_dfs_funcs_classes.cpp
MAIN_EXE_FILE := main
MAIN_UTEST_FILES := unit_tests_seqdfs.cpp seqdfs_vs_boost_dfs_funcs_classes.cpp
MAIN_TESTS_EXE_FILE := main_tests
MS_MIMALLOC_FOLDER := ms_mimalloc
MS_MIMALLOC_SO_OBJECT := $(MS_MIMALLOC_FOLDER)/out/secure/libmimalloc-secure.so

all: $(MAIN_SRC_FILES)
	g++ -ggdb3 -O3 -std=c++11 $^ -o $(MAIN_EXE_FILE)

tests: $(MAIN_UTEST_FILES)
	g++ -ggdb3 -O3 -std=c++11 $^ -o $(MAIN_TESTS_EXE_FILE)

clean:
	rm -rf $(MAIN_EXE_FILE) $(MAIN_TESTS_EXE_FILE)

run: clean all
	ulimit -s unlimited && ./$(MAIN_EXE_FILE)

run_mimalloc: clean all
	if [ -z "$(shell find ./ | grep "./$(MS_MIMALLOC_SO_OBJECT)" | sort | head -n1)" ]; then \
		./download_and_build_mimalloc.sh "$(MS_MIMALLOC_FOLDER)"; \
	fi
	ulimit -s unlimited && env MIMALLOC_SHOW_STATS=1 \
		LD_PRELOAD=./$(MS_MIMALLOC_SO_OBJECT) ./$(MAIN_EXE_FILE)

run_tests: clean tests
	./$(MAIN_TESTS_EXE_FILE)

