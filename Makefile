all: md5 vista slave

md5: md5.c
	gcc -Wall -std=c99 $< -o $@

vista: vista.c
	gcc -Wall -std=c99 $< -o $@

slave: slave.c
	gcc -Wall -std=c99 $< -o $@

clean:
	rm -f md5 vista slave

.PHONY: all clean

# uso -std=c99 porque a mí no me compilaban algunos archivos