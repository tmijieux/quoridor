DEPS=	$(wildcard ../src/*.c) \
	$(wildcard ../src/*.h) \
	$(wildcard ../src/strategies/*.c)
TARGET=	html/index.html

all: $(TARGET)

html/index.html: Doxyfile $(DEPS)
	doxygen Doxyfile

.PHONY: clean all

clean:
	$(RM) -r html/*

mrproper: clean
