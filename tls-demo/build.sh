# CC=g++
CC=clang++

$CC -O2 -shared -fPIC -ftls-model=initial-exec tls_test.cpp  -I . -o libtls_test.so
# clang++ -shared -fPIC tls_test.cpp  -I . -o libtls_test.so
echo "libtls_test.so"
readelf -s libtls_test.so | grep __tls_get_addr


$CC -O2 -shared -fPIC -ftls-model=initial-exec tls_demo.cpp -ltls_test  -I. -L.  -o libtls_demo.so
# clang++ -shared -fPIC tls_demo.cpp -ltls_test  -I. -L.  -o libtls_demo.so
echo "libtls_demo.so"
readelf -s libtls_demo.so | grep __tls_get_addr

$CC -O2 tls_exec.cpp -I. -L. -ltls_demo -ltls_test -o tls_exec

echo "tls_exec"
readelf -s tls_exec | grep __tls_get_addr
