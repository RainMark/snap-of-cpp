all:
	g++ -std=c++11 -shared -fPIC -D_GLIBCXX_USE_CXX11_ABI=0 foo.cpp -I. -o foo.so
	clang++ foo_main.cpp -D_GLIBCXX_USE_CXX11_ABI=1 -I. -ldl -o main

clean:
	rm -rf *.so main
