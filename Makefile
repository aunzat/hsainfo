all: hsainfo

CFLAGS := -std=c99 -I/opt/rocm/include
LDFLAGS := -L/opt/rocm/lib -lhsa-runtime64

hsainfo: hsainfo.o print_hsa.o hsa_error.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c %.h
	$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -rf -- *.o hsainfo

.PHONY: test
test: hsainfo
	./hsainfo
