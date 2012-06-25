#include <limits.h>
#include <fstream>
#include <string>

#include "include/sample1.h"
#include "gtest/gtest.h"
#include "include/proto_handler.h"
#include "test/interfaces.pb.h"

using namespace std;
using std::remove;

class FXRequestTest :  public ::testing::Test{
  protected:
  ProtoHandler phandler;
  mdfx_server::FXRequest request_in;
  mdfx_server::FXRequest request_out;
};

TEST_F(FXRequestTest,ReadProtobuf){
  fstream output("test/test_request", ios::out | ios::trunc | ios::binary);
  
  request_out.set_begin_timestamp(1309469186330);
  request_out.set_end_timestamp(1309469193440);
  request_out.set_max_rel_spread(0.01);
  
  request_out.add_nemo_list("EURUSD");
  request_out.add_nemo_list("USDGBP");
  request_out.SerializeToOstream(&output);
  string filename("test/test_request");
  EXPECT_TRUE( phandler.readRequestFromFile(filename));
  remove(filename.c_str());
  google::protobuf::ShutdownProtobufLibrary();
}
//
//TEST_F(BoookTest,WriteProtobuf){
//  fstream input("test_message", ios::in | ios::binary);
//}
// Tests Factorial().
// Tests factorial of negative numbers.
TEST(FactorialTest, Negative) {
  // This test is named "Negative", and belongs to the "FactorialTest"
  // test case.
  EXPECT_EQ(1, Factorial(-5));
  EXPECT_EQ(1, Factorial(-1));
  EXPECT_TRUE(Factorial(-10) > 0);

  // <TechnicalDetails>
  //
  // EXPECT_EQ(expected, actual) is the same as
  //
  //   EXPECT_TRUE((expected) == (actual))
  //
  // except that it will print both the expected value and the actual
  // value when the assertion fails.  This is very helpful for
  // debugging.  Therefore in this case EXPECT_EQ is preferred.
  //
  // On the other hand, EXPECT_TRUE accepts any Boolean expression,
  // and is thus more general.
  //
  // </TechnicalDetails>
}

// Tests factorial of 0.
TEST(FactorialTest, Zero) {
  EXPECT_EQ(1, Factorial(0));
}

// Tests factorial of positive numbers.
TEST(FactorialTest, Positive) {
  EXPECT_EQ(1, Factorial(1));
  EXPECT_EQ(2, Factorial(2));
  EXPECT_EQ(6, Factorial(3));
  EXPECT_EQ(40320, Factorial(8));
}
