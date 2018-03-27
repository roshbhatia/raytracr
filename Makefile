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
paralellsrc := $(srcdir)/grafix_engine_paralell.c
obj := $(patsubst %.c, %.o, $(src))
cleanfiles = $(main) $(paralell) $(obj)

$(srcdir)/%.o: $(srcdir)/%.c
	gcc -I $(includedir) -c $< -o $@
$(main): $(obj) $(mainsrc)
	gcc  -I $(includedir) $^ -lm -o $@
$(paralell): $(obj) $(paralellsrc)
	gcc  -I $(includedir) $^ -lm -o $@

all: $(main)
paralell: $(paralell)
clear:
	rm $(rootdir)/output/*.xwd
clean: 
	for f in $(clean_files); do \
		if test -f "$$f"; then $(RM) -v "$$f"; fi; \
	done


