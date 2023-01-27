#define BOOST_TEST_MODULE My Test
#include <stdbool.h>

#include <boost/test/included/unit_test.hpp>

#include <chip8.cpp>

BOOST_AUTO_TEST_CASE(test__give_valid_rom__loads_rom)
{
  chip_8 chip8;
  bool return_value;

  return_value = chip8.load_rom("foobar");

  BOOST_TEST(return_value);
}
