SRC_DIR = src
INC_DIR = include

CXXFLAGS += -g -Wall -Wextra

TESTS = mdfx_unittest 
SERVER = mdfx_server
CLIENT = mdfx_client

# Google Test headers.  
GTEST_HEADERS = lib/include/gtest/*.h \
                lib/include/gtest/internal/*.h

CPPFLAGS += -I. -Ilib/include -Ilib -ltcmalloc
BOOST = -lboost_system -lboost_filesystem -lboost_regex -lboost_date_time

.PHONY: all test server clean distlean

server: $(SERVER) 

test: $(TESTS)
	./bin/mdfx_unittest --gtest_output="xml:test/reports/"

client: $(CLIENT)

all : $(SERVER) $(TESTS) $(CLIENT)

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o $(INC_DIR)/*.gch 
	rm -f protoc_interfaces_middleman src/protobuf/interfaces.pb.*

testclean :
	rm -f  protoc_test_middleman test/interfaces.pb.cc test/interfaces.pb.h 

distclean : clean testclean
	rm -f bin/$(TESTS) bin/$(SERVER) bin/$(CLIENT)

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

protoc_test_middleman: config/interfaces.proto
	protoc --cpp_out=test -Iconfig config/interfaces.proto
		@touch protoc_test_middleman

sample1.o : $(SRC_DIR)/sample1.cc $(INC_DIR)/sample1.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(SRC_DIR)/sample1.cc

mdfx_unittest.o : test/mdfx_unittest.cc protoc_test_middleman proto_handler.o\
	$(INC_DIR)/sample1.h  $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c test/mdfx_unittest.cc `pkg-config --cflags --libs protobuf`$(BOOST)

mdfx_unittest : sample1.o mdfx_unittest.o csv_handler.o gtest_main.a
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) test/interfaces.pb.cc \
 	-pthread $^ proto_handler.o -o bin/$@ `pkg-config --cflags --libs protobuf` $(BOOST)

# Build Project

csv_handler.o : $(SRC_DIR)/csv_handler.cc 
	$(CXX) -I. -c $^ -o $@ $(BOOST)

protoc_interfaces_middleman: config/interfaces.proto
	protoc --cpp_out=src/protobuf -Iconfig config/interfaces.proto
	@touch protoc_interfaces_middleman

proto_handler.o: src/proto_handler.cc $(INC_DIR)/proto_handler.h protoc_interfaces_middleman
	pkg-config --cflags protobuf  # fails if protobuf is not installed
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/proto_handler.cc `pkg-config --cflags --libs protobuf`

mdfx_server: proto_handler.o src/server.cc 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/protobuf/interfaces.pb.cc src/server.cc proto_handler.o -o bin/$@ \
		-lzmq `pkg-config --cflags --libs protobuf`

mdfx_client: protoc_interfaces_middleman src/client.cc csv_handler.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/protobuf/interfaces.pb.cc src/client.cc -o bin/$@ \
		-lzmq `pkg-config --cflags --libs protobuf`
