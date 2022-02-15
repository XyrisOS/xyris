CXX:=clang++
KERNEL_DIR:=../../Kernel
KERNEL_SRCS:=$(KERNEL_DIR)/Memory/Heap.cpp

.PHONY: run
run: test-heap
	./$<

test-heap: test-heap.cpp $(KERNEL_SRCS)
	$(CXX) -o $@ -m32 -O1 -Wall -Wextra -std=gnu++17 -fsanitize=address -flto=full -I$(KERNEL_DIR) -DTESTING -DARCH_PAGE_SIZE=4096 $^
