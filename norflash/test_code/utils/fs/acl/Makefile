TARGETS= tacl_xattr.sh

all: $(TARGETS)

install:
	@set -e; for i in $(TARGETS) ; do ln -f $$i $(SUITE_DIR)/bin/$$i ; done

clean:
