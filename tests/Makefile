CXXFLAGS += -m32 -DTESTING -I../sysroot/usr/include/kernel/ -Wno-write-strings

run_tests: *.cpp 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $< ../kernel/lib/bitmap.cpp

.PHONY: test
test: run_tests
	@ ./run_tests

