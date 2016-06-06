all: prog test

.PHONY: doc prog rapport test

doc:
	make -C doc/

prog:
	make -C src/ clean
	make -C src/

test:
	make -C src/ clean
	make -C tst/

rapport:
	make -C rapport/

clean:
	make -C src/ clean
	make -C doc/ clean
	make -C tst/ clean

mrproper:
	make -C src/ mrproper
	make -C doc/ mrproper
	make -C tst/ mrproper
