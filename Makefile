CFLAGS=c11 -g -static
1cc: 1cc
test:1cc
	./test.sh
# bash -x test.sh when debag

clear:
	rm -f 1cc *.o *~ tmp*

.PHONY: test clean