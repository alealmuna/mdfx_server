SRC_DIR = src
INC_DIR = include

CXXFLAGS += -g -Wall -Wextra

TESTS = sample1_unittest 
SERVER = mdfx_server

# Google Test headers.  
GTEST_HEADERS = lib/include/gtest/*.h \
                lib/include/gtest/internal/*.h

CPPFLAGS += -Iinclude -Ilib/include -Ilib

.PHONY: all test server clean distlean

server: $(SERVER) 

test: $(TESTS)

all : $(SERVER) $(TESTS)

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o $(INC_DIR)/*.gch

testclean :
	rm -f  protoc_test_middleman test/test_message.pb.cc test/test_message.pb.h 

distclean :
	make clean
	make testclean
	rm -f bin/$(TESTS) bin/$(SERVER)

# Builds gtest.a and gtest_main.a.

GTEST_SRCS_ = lib/src/gtest/*.cc lib/src/gtest/*.h $(GTEST_HEADERS)

gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c \
            lib/src/gtest/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c \
           lib/src/gtest/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.

protoc_test_middleman: test/test_message.proto
	protoc --cpp_out=test -Itest test/test_message.proto
		@touch protoc_test_middleman

sample1.o : $(SRC_DIR)/sample1.cc $(INC_DIR)/sample1.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/sample1.cc

sample1_unittest.o : test/sample1_unittest.cc \
	$(INC_DIR)/sample1.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c test/sample1_unittest.cc

sample1_unittest : sample1.o sample1_unittest.o gtest_main.a protoc_test_middleman
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) test/test_message.pb.cc \
 	-pthread sample1.o sample1_unittest.o gtest_main.a \
  -o bin/$@ `pkg-config --cflags --libs protobuf`


# Build Project

proto_handler.o: src/proto_handler.cc $(INC_DIR)/proto_handler.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/proto_handler.cc

mdfx_server: proto_handler.o src/server.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o bin/$@
