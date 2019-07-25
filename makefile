TARGET := depth
LIBS := -lpng -lpthread

DEBUG := $(if $(shell git symbolic-ref --short HEAD | grep master), , -g)
SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst src/%.cpp, build/%.o, $(SOURCES))
CC := g++

.PHONY: clean, install, uninstall

$(TARGET): $(OBJECTS) $(SHADERS)
	$(CC) -o $@ $(OBJECTS) $(LIBS) -no-pie

clean:
	$(RM) -r build/
	$(RM) -r $(TARGET)

install:
	sudo cp $(TARGET) /usr/local/bin/

uninstall:
	sudo $(RM) /usr/local/bin/$(TARGET)

define OBJECT_RULE
build/$(subst $() \,,$(shell $(CC) -MM $(1)))
	mkdir -p build/
	$$(CC) $(DEBUG) -c -o $$@ $$<
endef
$(foreach src, $(SOURCES), $(eval $(call OBJECT_RULE,$(src))))
