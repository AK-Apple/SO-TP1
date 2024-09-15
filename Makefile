LINKEDITABLE = pipe_group_adt.c result_adt.c

PARAMS = -Wall -std=gnu11 -lrt -lpthread -g

all: vista slave md5

md5: md5.c
	gcc $(PARAMS) $(LINKEDITABLE) $< -o $@

vista: vista.c
	gcc $(PARAMS) result_adt.c $< -o $@

slave: slave.c
	gcc $(PARAMS) $< -o $@

clean:
	rm -f md5 vista slave md5_result.txt

.PHONY: all clean