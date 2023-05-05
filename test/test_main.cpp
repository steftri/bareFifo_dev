#include <Arduino.h>
#include <unity.h>

#include "barefifo.h"


uint8_t gau8_FifoBuffer[10]={0xEF, 0, 0, 0, 0, 0, 0, 0, 0, 0xEF};
BareFifo MyFifo(gau8_FifoBuffer+1, 8);


void setUp(void) 
{
    // set stuff up here
}

void tearDown(void) 
{
    // clean stuff up here
}



void test_empty_fifo(void) 
{
  char ac_Buffer[8];

  TEST_ASSERT_TRUE(MyFifo.isEmpty());
  TEST_ASSERT_EQUAL_INT32(0, MyFifo.available());
  TEST_ASSERT_EQUAL_INT32(8, MyFifo.availableForWrite());
}



void test_fifo_almost_empty_or_full(void) 
{
  uint8_t au8_Buffer[8];

  MyFifo.write((const uint8_t*)"1", 1);
  TEST_ASSERT_FALSE(MyFifo.isEmpty());
  TEST_ASSERT_TRUE(MyFifo.isAlmostEmpty());
  TEST_ASSERT_FALSE(MyFifo.isAlmostFull()); 
  TEST_ASSERT_FALSE(MyFifo.isFull()); 

  MyFifo.write((const uint8_t*)"2", 1);
  TEST_ASSERT_FALSE(MyFifo.isEmpty());
  TEST_ASSERT_FALSE(MyFifo.isAlmostEmpty());
  TEST_ASSERT_FALSE(MyFifo.isAlmostFull()); 
  TEST_ASSERT_FALSE(MyFifo.isFull()); 

  MyFifo.write((const uint8_t*)"3456", 4);
  TEST_ASSERT_FALSE(MyFifo.isEmpty());
  TEST_ASSERT_FALSE(MyFifo.isAlmostEmpty());
  TEST_ASSERT_FALSE(MyFifo.isAlmostFull()); 
  TEST_ASSERT_FALSE(MyFifo.isFull()); 

  MyFifo.write((const uint8_t*)"7", 1);
  TEST_ASSERT_FALSE(MyFifo.isEmpty());
  TEST_ASSERT_FALSE(MyFifo.isAlmostEmpty());
  TEST_ASSERT_TRUE(MyFifo.isAlmostFull()); 
  TEST_ASSERT_FALSE(MyFifo.isFull()); 
 
  MyFifo.write((const uint8_t*)"7", 1);
  TEST_ASSERT_FALSE(MyFifo.isEmpty());
  TEST_ASSERT_FALSE(MyFifo.isAlmostEmpty());
  TEST_ASSERT_TRUE(MyFifo.isAlmostFull()); 
  TEST_ASSERT_TRUE(MyFifo.isFull()); 
}



void test_fifo_reset(void) 
{
  uint8_t au8_Buffer[8];

  MyFifo.write((const uint8_t*)"1234", 4);
  MyFifo.read(au8_Buffer, 2);
  MyFifo.reset();

  TEST_ASSERT_TRUE(MyFifo.isEmpty());
  TEST_ASSERT_EQUAL_INT32(0, MyFifo.available());  
}



void test_fifo_overflow(void) 
{
  uint8_t au8_Buffer[8];

  MyFifo.reset();
  MyFifo.write((const uint8_t*)"12345678", 8);
  TEST_ASSERT_FALSE(MyFifo.isOverflow());

  MyFifo.reset();
  MyFifo.write((const uint8_t*)"1234567", 7);
  TEST_ASSERT_FALSE(MyFifo.isOverflow());
  MyFifo.write((const uint8_t*)"89", 2);
  TEST_ASSERT_TRUE(MyFifo.isOverflow());  

  MyFifo.reset();
  TEST_ASSERT_FALSE(MyFifo.isOverflow());
}



void test_fifo_write_simple(void) 
{
  uint8_t au8_Buffer[8];
  uint32_t u32_Read, u32_Written;

  MyFifo.reset();

  u32_Written = MyFifo.write((const uint8_t*)"1234", 4);
  TEST_ASSERT_EQUAL_INT32(4, u32_Written);
  TEST_ASSERT_EQUAL_INT32(4, MyFifo.available()); 
  u32_Read = MyFifo.read(au8_Buffer, 8);
  TEST_ASSERT_EQUAL_INT32(4, u32_Read);
  TEST_ASSERT_EQUAL_UINT8_ARRAY((const uint8_t*)"1234", au8_Buffer, 4); 

  u32_Written = MyFifo.write((const uint8_t*)"5678", 4);
  TEST_ASSERT_EQUAL_INT32(4, u32_Written);
  TEST_ASSERT_EQUAL_INT32(4, MyFifo.available()); 
  u32_Read = MyFifo.read(au8_Buffer, 8);
  TEST_ASSERT_EQUAL_INT32(4, u32_Read);
  TEST_ASSERT_EQUAL_UINT8_ARRAY((const uint8_t*)"5678", au8_Buffer, 4); 

  u32_Written = MyFifo.write((const uint8_t*)"90AB", 4);
  TEST_ASSERT_EQUAL_INT32(4, u32_Written);
  TEST_ASSERT_EQUAL_INT32(4, MyFifo.available()); 
  u32_Read = MyFifo.read(au8_Buffer, 8);
  TEST_ASSERT_EQUAL_INT32(4, u32_Read);
  TEST_ASSERT_EQUAL_UINT8_ARRAY((const uint8_t*)"90AB", au8_Buffer, 4);   
}



void test_fifo_write_wraparound_1(void) 
{
  uint8_t au8_Buffer[8];
  uint32_t u32_Read, u32_Written;

  MyFifo.reset();
  u32_Written = MyFifo.write((const uint8_t*)"123456", 6);  // FIFO content shoud be "123456.."
  u32_Read = MyFifo.read(au8_Buffer, 8);                    // FIFO content shoud be "........"
  TEST_ASSERT_EQUAL_INT32(6, u32_Read);
  TEST_ASSERT_EQUAL_UINT8_ARRAY((const uint8_t*)"123456", au8_Buffer, 6); 

  u32_Written = MyFifo.write((const uint8_t*)"7890", 4);   // FIFO content shoud be "90....78"
  u32_Read = MyFifo.read(au8_Buffer, 8);                   // FIFO content shoud be "........"
  TEST_ASSERT_EQUAL_INT32(4, u32_Read);
  TEST_ASSERT_EQUAL_UINT8_ARRAY((const uint8_t*)"7890", au8_Buffer, 4); 
}


void test_fifo_write_wraparound_2(void) 
{
  uint8_t au8_Buffer[8];
  uint32_t u32_Read, u32_Written;

  MyFifo.reset();
  u32_Written = MyFifo.write((const uint8_t*)"123456", 6);  // FIFO content shoud be "123456.."
  u32_Read = MyFifo.read(au8_Buffer, 8);                    // FIFO content shoud be "........"
  TEST_ASSERT_EQUAL_INT32(6, u32_Read);
  TEST_ASSERT_EQUAL_UINT8_ARRAY((const uint8_t*)"123456", au8_Buffer, 6); 

  u32_Written = MyFifo.write((const uint8_t*)"7890", 4);   // FIFO content shoud be "90....78"
  TEST_ASSERT_EQUAL_INT32(4, u32_Written);

  u32_Written = MyFifo.write((const uint8_t*)"ABCD", 4);   // FIFO content shoud be "90ABCD78"
  u32_Read = MyFifo.read(au8_Buffer, 8);                   // FIFO content shoud be "........"
  TEST_ASSERT_EQUAL_INT32(8, u32_Read);
  TEST_ASSERT_EQUAL_UINT8_ARRAY((const uint8_t*)"7890ABCD", au8_Buffer, 8);   
}


void test_fifo_readwrite(void) 
{
  uint8_t au8_Buffer[10];
  uint32_t u32_Read, u32_Written;

  MyFifo.reset();
  u32_Written = MyFifo.write((const uint8_t*)"1234", 4);  // FIFO content shoud be "1234...."
  TEST_ASSERT_EQUAL_INT32(4, u32_Written);
  TEST_ASSERT_EQUAL_INT32(4, MyFifo.available());  

  u32_Written = MyFifo.write((const uint8_t*)"567", 3);   // FIFO content shoud be "1234567."
  TEST_ASSERT_EQUAL_INT32(3, u32_Written);
  TEST_ASSERT_EQUAL_INT32(7, MyFifo.available());  

  u32_Read = MyFifo.read(au8_Buffer, 2);                  // FIFO content shoud be "..34567."
  TEST_ASSERT_EQUAL_INT32(2, u32_Read);
  TEST_ASSERT_EQUAL_INT32(5, MyFifo.available()); 
  TEST_ASSERT_EQUAL_UINT8_ARRAY((const uint8_t*)"12", au8_Buffer, 2); 

  u32_Written = MyFifo.write((const uint8_t*)"89", 2);   // FIFO content shoud be "9.345678"
  TEST_ASSERT_EQUAL_INT32(2, u32_Written);
  TEST_ASSERT_EQUAL_INT32(7, MyFifo.available());

  u32_Read = MyFifo.read(au8_Buffer, 8);                 // FIFO content shoud be "........"
  TEST_ASSERT_EQUAL_INT32(7, u32_Read);
  TEST_ASSERT_EQUAL_INT32(0, MyFifo.available());
  TEST_ASSERT_EQUAL_UINT8_ARRAY((const uint8_t*)"3456789", au8_Buffer, 7);

  u32_Written = MyFifo.write((const uint8_t*)"0A", 2);   // FIFO content shoud be ".0A....."
  TEST_ASSERT_EQUAL_INT32(2, u32_Written);
  TEST_ASSERT_EQUAL_INT32(2, MyFifo.available());
  TEST_ASSERT_FALSE(MyFifo.isOverflow());  

  u32_Written = MyFifo.write((const uint8_t*)"BCDEFGH", 7);  // FIFO content shoud be "G0ABCDEF"
  TEST_ASSERT_EQUAL_INT32(6, u32_Written);
  TEST_ASSERT_EQUAL_INT32(8, MyFifo.available());
  TEST_ASSERT_TRUE(MyFifo.isOverflow());
  TEST_ASSERT_EQUAL_INT32(0, MyFifo.availableForWrite());

  u32_Written = MyFifo.write((const uint8_t*)"I", 1);   // FIFO content shoud be "G0ABCDEF" (unchanged)
  TEST_ASSERT_EQUAL_INT32(0, u32_Written);
  TEST_ASSERT_EQUAL_INT32(8, MyFifo.available());
  TEST_ASSERT_EQUAL_INT32(0, MyFifo.availableForWrite());

  u32_Read = MyFifo.read(au8_Buffer, 10);
  TEST_ASSERT_EQUAL_INT32(8, u32_Read);
  TEST_ASSERT_EQUAL_INT32(0, MyFifo.available());
  TEST_ASSERT_EQUAL_UINT8_ARRAY((const uint8_t*)"0ABCDEFG", au8_Buffer, 8);
}



void test_fifo_electric_fence(void) 
{
  TEST_ASSERT_EQUAL_INT8(0xEF, gau8_FifoBuffer[0]);
  TEST_ASSERT_EQUAL_INT8(0xEF, gau8_FifoBuffer[9]);
}



void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();

    RUN_TEST(test_empty_fifo);
    RUN_TEST(test_fifo_almost_empty_or_full);
    RUN_TEST(test_fifo_reset);
    RUN_TEST(test_fifo_overflow);
    RUN_TEST(test_fifo_write_simple);
    RUN_TEST(test_fifo_write_wraparound_1);
    RUN_TEST(test_fifo_write_wraparound_2);
    RUN_TEST(test_fifo_readwrite);

    RUN_TEST(test_fifo_electric_fence);

    UNITY_END(); // stop unit testing
}

void loop()
{
}