CC := g++
exe := main
#obj := main.o statistic.o evaluate.o utility.o
CFLAGS := -std=c++17 -O2 -Wall -Wextra -fsanitize=undefined -fsanitize=address
LINK := -lm -ldl -lreadline -lpthread
LIB := lib/libabc.a
#INCLUDES :=

src_dir = ./src
obj_dir = ./obj

src = $(wildcard $(src_dir)/*.cpp)
obj = $(patsubst $(src_dir)/%.cpp, $(obj_dir)/%.o, $(src))

all:$(obj) $(LIB) lib/libcadical.a
	mkdir -p $(obj_dir)
	cd ./lib/aiger && ./configure.sh && make
	mv lib/aiger/aiger.o obj/
	$(CC) $(CFLAGS) -o $(exe) $(obj) ./obj/aiger.o $(LIB) lib/libcadical.a  $(LINK)

$(obj_dir)/%.o: $(src_dir)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

lib/libabc.a:
	cd lib/abc/ && make -j 20 libabc.a
	mv lib/abc/libabc.a lib/
	cd lib/cadical && make clean && ./configure && make
	mv lib/cadical/build/libcadical.a lib/
clean:remove
	rm -rf $(obj) $(exe)

remove:
	rm -f nCir* output-* rm *.aig rm *.cnf rm stdoutOutput.txt