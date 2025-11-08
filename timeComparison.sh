g++ timeComparison.cpp \
    $(find ./cashFriendlyList -name "*.cpp" ! -name "main.cpp") \
    $(find ./classicalList -name "*.cpp" ! -name "main.cpp") \
    $(find ./general -name "*.cpp") \
    -I./general -I./cashFriendlyList -I./classicalList \
    -o timeComp_perf.out \
    -std=c++17 -O3 -march=native -flto \
    -DNDEBUG \
