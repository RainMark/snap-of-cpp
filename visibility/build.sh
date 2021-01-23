rm -rf *.o *.a *.so

CC=g++
# FLAGS="-O0 -std=c++1y -fno-inline-small-functions -fPIC -fvisibility=hidden"
FLAGS="-O2 -std=c++1y -fno-inline-small-functions -fPIC"

set -x

$CC -c $FLAGS foo.cpp -I. -o foo.o
$CC -shared $FLAGS foo.o -I. -o libfoo.so
# $CC -c -std=c++11 -fPIC foo.cpp -I. -o foo.o
# $CC -shared foo.o -I. -o libfoo.so
# readelf -s libfoo.so | grep get
ar rcs libfoo.a foo.o
# readelf -s libfoo.a | grep get

cd foo2
$CC -c -std=c++11 -fPIC foo2.cpp -I. -o foo2.o
$CC -shared -std=c++11 -fPIC foo2.o -I. -o libfoo2.so
# readelf -s libfoo2.so | grep get
ar rcs libfoo2.a foo2.o
# readelf -s libfoo2.a | grep get
cd -

# 使用 version script 时，可以不要求其他依赖的 .a 文件是 -fvisibility=hidden 编译产出
# 否咋需要依赖项目的 .o 文件都是 -fvisibility=hidden 编译才行
# 总结：A项目产出 so，链接依赖项目的 .a 文件，开启 version script

$CC -c $FLAGS bar.cpp -I. -o bar.o
$CC -shared -Wl,--version-script=bar.lds $FLAGS bar.o -I. -o libbar.so libfoo.a
# $CC -shared $FLAGS bar.o -I. -o libbar.so libfoo.a
# $CC -shared $FLAGS bar.o -I. -o libbar.so

readelf -s libbar.so | grep get
ar rcs libbar.a bar.o foo.o
readelf -s libbar.a | grep get

objcopy --localize-hidden foo.o lfoo.o
objcopy --localize-hidden bar.o lbar.o
ar rcs liblbar.a lbar.o lfoo.o

$CC -std=c++11 main.cpp -I. -L. -Lfoo2 -lfoo2 -lbar
# $CC -std=c++11 main.cpp -I. -Lfoo2 libbar.a -lfoo2
# $CC -std=c++11 main.cpp -I. -Lfoo2 -lfoo2 libbar.a
# $CC -std=c++11 main.cpp -I. liblbar.a -Lfoo2 -lfoo2

LD_LIBRARY_PATH=.:foo2 ./a.out


