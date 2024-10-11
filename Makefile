CCFLAG += -MMD -MP -g3 $(shell sdl2-config --cflags)

rootdir = $(shell pwd)

srcdir = $(rootdir)/src

includedir = $(srcdir)

src:= \
	$(srcdir)/D3d_matrix.c \
	$(srcdir)/parametric_equations.c \
	$(srcdir)/xwd_tools.c \

main := engine.out

mainsrc := $(srcdir)/main.c

playxwdssrc := $(srcdir)/play_xwds.c

obj := $(src:.c=.o)

dep := $(obj:.o=.d)

cleanfiles = $(main) $(paralell) $(obj) $(dep)

$(srcdir)/%.o: $(srcdir)/%.c
	$(CC) $(CCFLAG) -I $(includedir) -c $< -o $@

$(main): $(obj) $(mainsrc)
	$(CC) $(CCFLAG) -I $(includedir) $^ -lm $(shell sdl2-config --libs) -o $@

$(_play_xwds): $(obj) $(playxwds) 
	$(CC) $(CCFLAG) -I $(includedir) $^ -lm -o $@

all: $(main)

play_xwds: $(_play_xwds)

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
