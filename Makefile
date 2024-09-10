LINKEDITABLE = PipeGroupADT.c

PARAMS = -Wall -std=c99 -lrt -pthread -g

all: md5 vista slave junior_slave PipeGroupTest

md5: md5.c
	gcc $(PARAMS) $< -o $@

vista: vista.c
	gcc $(PARAMS) ResultADT.c $< -o $@

slave: slave.c
	gcc $(PARAMS) $< -o $@

junior_slave: junior_slave.c
	gcc $(PARAMS) $< -o $@ 

PipeGroupTest: PipeGroupTest.c
	gcc $(PARAMS) ResultADT.c $< -o $@


clean:
	rm -f md5 vista slave junior_slave PipeGroupTest

.PHONY: all clean

# uso -std=c99 y -pthread porque a mí no me compilaban algunos archivos

# TODO: por alguna razón misteriosa, los LINKEDITABLE se linkeditan solos
# TODO: sacar los archivos extra del .gitignore