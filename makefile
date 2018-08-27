CC=g++
CFLAGS=-I.

build: depth.cpp
	@echo "Depth building ..."
	@$(if $(shell dpkg -l | grep -E '^ii' | grep g++), , sudo apt -y -qq install g++)
	@$(if $(shell dpkg -l | grep -E '^ii' | grep libpng-dev), , sudo apt -y -qq install libpng-dev)
	@$(CC) -o depth depth.cpp -lpng
	@echo "Depth built."

.PHONY: clean, install, uninstall

clean:
	@echo "Depth cleaning ..."
	@echo "Depth cleaned."

install:
	@echo "Depth installing ..."
	@sudo cp depth /usr/local/sbin/
	@echo "Depth installed."

uninstall:
	@echo "Depth uninstalling ..."
	@echo "Depth uninstalled."
