CCFLAG += -MMD -MP -g3

rootdir = $(shell pwd)

srcdir = $(rootdir)/src

includedir = $(srcdir)

src:= \
	$(srcdir)/D3d_matrix.c \
	$(srcdir)/parametric_equations.c \
	$(srcdir)/xwd_tools.c \

main := engine.out

mainsrc := $(srcdir)/grafix_engine.c

parallel:= singleframe.out

paralellmainsrc := $(srcdir)/grafix_engine_paralell.c

obj := $(src:.c=.o)

dep := $(obj:.o=.d)

cleanfiles = $(main) $(paralell) $(obj) $(dep)

$(srcdir)/%.o: $(srcdir)/%.c
	$(CC) $(CCFLAG) -I $(includedir) -c $< -o $@

$(main): $(obj) $(mainsrc)
	$(CC) $(CCFLAG) -I $(includedir) $^ -lm -o $@

$(_paralell): $(obj) $(paralellmainsrc) 
	$(CC) $(CCFLAG) -I $(includedir) $^ -lm -o $@

all: $(main)

paralell: $(_paralell)

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


