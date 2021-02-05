#/bin/bash

# carefull !!!
# please backup libgcc.a firstly

libgcc=`find /usr/ -name libgcc.a`

g++ -c morestack.S -o morestack.o
ar -r $libgcc morestack.o
