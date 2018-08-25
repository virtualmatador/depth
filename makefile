CC=g++
CFLAGS=-I.

build: depth.cpp
	@echo "Building ..."
	@sudo apt -y -qq install g++ libpng-dev
	@$(CC) -o depth depth.cpp -lpng
	@echo "Built."

.PHONY: clean, install, uninstall

clean:
	@echo "Cleaning ..."
	@echo "Cleaned."

install:
	@echo "Installing ..."
	@sudo cp depth /usr/local/sbin/
	@echo "Installed."

uninstall:
	@echo "Uninstalling ..."
	@echo "Uninstalled."
