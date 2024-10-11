CCFLAG += -MMD -MP -g3 $(shell sdl2-config --cflags)

rootdir = $(shell pwd)

srcdir = $(rootdir)/src

includedir = $(srcdir)

src:= \
	$(srcdir)/D3d_matrix.c \
	$(srcdir)/parametric_equations.c \

main := engine.out

mainsrc := $(srcdir)/main.c

obj := $(src:.c=.o)

dep := $(obj:.o=.d)

cleanfiles = $(main) $(obj) $(dep)

$(srcdir)/%.o: $(srcdir)/%.c
	$(CC) $(CCFLAG) -I $(includedir) -c $< -o $@

$(main): $(obj) $(mainsrc)
	$(CC) $(CCFLAG) -I $(includedir) $^ -lm $(shell sdl2-config --libs) -o $@

all: $(main)

clear:
	rm $(rootdir)/output/*.xwd

delexec:
	rm $(rootdir)/*.out

clean: 
	for f in $(cleanfiles); do \
		if test -f "$$f"; then $(RM) -v "$$f"; fi; \
	done

-include $(dep)
.PHONY: all clean
