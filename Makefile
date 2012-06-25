SRC_DIR = src
INC_DIR = include

CXXFLAGS += -g -Wall -Wextra

TESTS = mdfx_unittest 
SERVER = mdfx_server

# Google Test headers.  
GTEST_HEADERS = lib/include/gtest/*.h \
                lib/include/gtest/internal/*.h

CPPFLAGS += -I. -Ilib/include -Ilib

.PHONY: all test server clean distlean

server: $(SERVER) 

test: $(TESTS)

all : $(SERVER) $(TESTS)

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o $(INC_DIR)/*.gch 
	rm -f protoc_interfaces_middleman src/protobuf/interfaces.pb.*

testclean :
	rm -f  protoc_test_middleman test/book.pb.cc test/book.pb.h 

distclean : clean testclean
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

protoc_test_middleman: test/book.proto
	protoc --cpp_out=test -Itest test/book.proto
		@touch protoc_test_middleman

sample1.o : $(SRC_DIR)/sample1.cc $(INC_DIR)/sample1.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/sample1.cc

mdfx_unittest.o : test/mdfx_unittest.cc protoc_test_middleman\
	$(INC_DIR)/sample1.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c test/mdfx_unittest.cc `pkg-config --cflags --libs protobuf`

mdfx_unittest : sample1.o mdfx_unittest.o gtest_main.a 
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) test/book.pb.cc \
 	-pthread sample1.o mdfx_unittest.o gtest_main.a \
  -o bin/$@ `pkg-config --cflags --libs protobuf`


# Build Project

protoc_interfaces_middleman: config/interfaces.proto
	protoc --cpp_out=src/protobuf -Iconfig config/interfaces.proto
	@touch protoc_interfaces_middleman

proto_handler.o: src/proto_handler.cc $(INC_DIR)/proto_handler.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/proto_handler.cc

mdfx_server: proto_handler.o src/server.cc protoc_interfaces_middleman
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/server.cc proto_handler.o -o bin/$@ \
  `pkg-config --cflags --libs protobuf`
