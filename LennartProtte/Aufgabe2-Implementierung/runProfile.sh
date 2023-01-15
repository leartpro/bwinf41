g++ main.cpp -o main && make main && ./main
sudo perf record -g ./main -fno-omit-framepointer --call-graph dwarf
