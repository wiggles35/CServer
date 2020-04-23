CC=		gcc
CFLAGS=		-g -Wall -Werror -std=gnu99 -Iinclude
LD=		gcc
LDFLAGS=	-L.
AR=		ar
ARFLAGS=	rcs
TARGETS=	bin/spidey

all:		$(TARGETS)

clean:
	@echo Cleaning...
	@rm -f $(TARGETS) lib/*.a src/*.o *.log *.input

.PHONY:		all test clean

# TODO: Add rules for bin/spidey, lib/libspidey.a, and any intermediate objects
bin/spidey: src/spidey.o lib/libspidey.a 
	@echo Linking $@...
	@$(LD) $(LDFLAGS) -o $@ $^

lib/libspidey.a: src/forking.o src/request.o src/single.o src/socket.o src/utils.o src/handler.o
	@echo Linking $@...
	@$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	@echo Compiling $@...
	@$(CC) $(CFLAGS) -c -o $@ $^


