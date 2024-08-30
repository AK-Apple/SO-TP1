all: md5 vista slave junior_slave

md5: md5.c
	gcc -Wall -std=c99 $< -o $@ -g

vista: vista.c
	gcc -Wall -std=c99 $< -o $@ -g

slave: slave.c
	gcc -Wall -std=c99 $< -o $@ -g

junior_slave: junior_slave.c
	gcc -Wall -std=c99 $< -o $@ -g

clean:
	rm -f md5 vista slave junior_slave

.PHONY: all clean

# uso -std=c99 porque a mí no me compilaban algunos archivos