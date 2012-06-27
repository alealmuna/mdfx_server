#include <limits.h>
#include "Readcsv.h"
#include "gtest/gtest.h"
/*
TEST(Readcsv, Lectura) {
  // This test is named "Lectura", and belongs to the "Readcsv"
  EXPECT_EQ(["2011/01/03","17:05:17.562","12.144","10000000","",""], Readcsv("csvtest.csv"));
}
*/
TEST(Readcsv, Cantidad) {
  EXPECT_EQ(6, Readcsv("csvtest.csv").size());	
}
/*
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
*/
