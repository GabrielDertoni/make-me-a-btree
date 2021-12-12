FORMATTER := cat

.PHONY: all
all: btree.c btree.h

btree.h: mk_bt.h
	@$(CC) $(DEFINES) -DBT_GENERATE -DBT_DECL_ONLY -E $^ \
		| sed 's/^#.*$$//g'                              \
		| sed 's/^!\(.*\)$$/\1/g'                        \
		| sed '/^$$/{N;/^\n$$/d;}'                       \
		| $(FORMATTER)                                   > $@

btree.c: mk_bt.h
	@$(CC) $(DEFINES) -DBT_GENERATE -E $^                \
		| sed 's/^#.*$$//g'                              \
		| sed 's/^!\(.*\)$$/\1/g'                        \
		| sed '/^$$/{N;/^\n$$/d;}'                       \
		| $(FORMATTER)                                   > $@
