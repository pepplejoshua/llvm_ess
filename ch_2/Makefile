target:
		clang++ -O3 main.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o main -arch arm64

clean:
		rm -rf main main.dSYM