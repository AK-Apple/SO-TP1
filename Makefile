LINKEDITABLE = PipeGroupADT.c ResultADT.c

PARAMS = -Wall -std=c99 -lrt -lpthread -g 

all: vista slave md5

md5: md5.c
	gcc $(PARAMS) $(LINKEDITABLE) $< -o $@

vista: vista.c
	gcc $(PARAMS) ResultADT.c $< -o $@

slave: slave.c
	gcc $(PARAMS) $< -o $@

clean:
	rm -f md5 vista slave md5_result.txt

.PHONY: all clean