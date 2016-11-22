all: hsainfo

hsainfo: hsainfo.o print_hsa.o hsa_error.o
	$(CC) -std=c99 -o $@ -L/opt/rocm/lib -lhsa-runtime64  $^

%.o: %.c
	$(CC) -c -I/opt/rocm/include -o $@ $< -std=c99

.PHONY: clean
clean:
	rm -rf -- *.o hsainfo

.PHONY: test
test: hsainfo
	./hsainfo
