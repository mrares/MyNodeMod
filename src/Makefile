PREFIX=/usr/local

all: _hello.node

_hello.node: hello.cc
	node-waf configure build

install: _hello.node
	cp _hello.node "$(PREFIX)/lib/node/libraries"

uninstall:
	rm -f "$(PREFIX)/lib/node/libraries/_hello.node"

clean:
	rm -f _hello.node
	rm -rf build

.PHONY: clean install
