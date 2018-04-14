cat_proc: cat_proc.c lkl/tools/lkl/liblkl.a
	gcc -Wall -Wextra -Ilkl/tools/lkl/include $^ -lpthread -lrt -o $@


vfork: vfork.c lkl/tools/lkl/liblkl.a
	gcc -Wall -Wextra -Ilkl/tools/lkl/include $^ -lpthread -lrt -o $@

lkl:
	git clone https://github.com/lkl/linux.git lkl

lkl/tools/lkl/liblkl.a: lkl
	cd lkl; patch -p1 < $(CURDIR)/patch/vfork.patch
	make -C lkl/tools/lkl
