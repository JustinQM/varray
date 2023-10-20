CC = gcc
CFLAGS=-Wall -O3
TARGET=varray

$(TARGET): main.c libvarray.a
	$(CC) $(CFLAGS) $^ -o $@

debug: main.c libvarray.a
	$(CC) -Wall -O1 -g  $^ -o $@

varray.o: varray.c
	$(CC) $(CFLAGS) -c $< -o $@

libvarray.a: varray.o 
	ar rcs $@ $<

install: libvarray.a
	cp -f ./libvarray.a /usr/local/lib
	cp -f ./varray.h /usr/local/include


