#include <cstdint>
#include "Tools.h"
#include <cstdlib>
#include <string>

/* buildLong:
 * builds a 64-bit long out of an array of 8 bytes
 *
 * for example, suppose bytes[0] == 0x12
 *              and     bytes[1] == 0x34
 *              and     bytes[2] == 0x56
 *              and     bytes[3] == 0x78
 *              and     bytes[4] == 0x9a
 *              and     bytes[5] == 0xbc
 *              and     bytes[6] == 0xde
 *              and     bytes[7] == 0xf0
 * then buildLong(bytes) returns 0xf0debc9a78563412
 *
 * @param array of 8 bytes
 * @return uint64_t where the low order byte is bytes[0] and
 *         the high order byte is bytes[7]
 */
uint64_t Tools::buildLong(uint8_t bytes[])
{
	uint64_t num = bytes[7];
	for (int i = 6; i >= 0; i--)
	{
		num = num << SHIFTVAL; 
		num = num | bytes[i];
		//printf("Byte: %2x\n", bytes[i-1]);
		//printf("First: %016x\n", num);
		//printf("Second: %016x\n", num);
	}
   return num;
}

/* getByte:
 * accepts as input an uint64_t and returns the designated byte
 * within the uint64_t; returns 0 if the indicated byte number
 * is out of range 
 *
 * for example, getByte(0x1122334455667788, 7) returns 0x11
 *              getByte(0x1122334455667788, 1) returns 0x77
 *              getByte(0x1122334455667788, 8) returns 0
 *
 * @param uint64_t source that is the source data
 * @param int32_t byteNum that indicates the byte to return (0 through 7)
 * @return 0 if byteNum is out of range
 *         byte 0, 1, .., or 7 of source if byteNum is within range
 */
uint64_t Tools::getByte(uint64_t source, int32_t byteNum)
{
	uint8_t byte = 0;
	uint64_t mask = 0xff;
	if(byteNum >= 0 && byteNum <= 7)
	{
		byte = (source >> SHIFTVAL * byteNum) & mask;
	}
	
   	return byte;
}

/* getBits:
 * accepts as input an uint64_t and returns the bits low through 
 * high of the uint64_t.  bit 0 is the low order bit and bit 63
 * is the high order bit. returns 0 if the low or high bit numbers 
 * are out of range
 *
 * for example, getBits(0x8877665544332211, 0, 7) returns 0x11
 *              getBits(0x8877665544332211, 4, 11) returns 0x21
 *              getBits(0x8877665544332211, 0, 63) returns 0x8877665544332211
 *
 * @param uint64_t source that holds the bits to be grabbed and 
 *        returned
 * @param int32_t low that is the bit number of the lowest numbered
 *        bit to be returned
 * @param int32_t high that is the bit number of the highest numbered
 *        bit to be returned
 * @return an uint64_t that holds a subset of the source bits
 *         that is returned in the low order bits; 0 if low or high 
 *         is out of range
 */
uint64_t Tools::getBits(uint64_t source, int32_t low, int32_t high)
{
	uint64_t num = 0;
	if((low >= 0 && low <= 63)&&(high >= 0 && high <= 63))
	{
		num = source << (63 - high);
		num = num >> (63 - high);
		num = num >> low;
		//printf("S: %016x\n", source);
		//printf("F: %016x\n", num);
	}
   return num;
}

/* setBits:
 * sets the bits of source in the range specified by the low and high
 * parameters to 1 and returns that value. returns source if the low or high
 * bit numbers are out of range
 *
 * for example, setBits(0x1122334455667788, 0, 7) returns 0x11223344556677ff
 *              setBits(0x1122334455667788, 8, 0xf) returns 0x112233445566ff88
 *              setBits(0x1122334455667788, 8, 64) returns 0x1122334455667788
 *                      note: 64 is out of range
 *
 * @param uint64_t source 
 * @param int32_t low that is the bit number of the lowest numbered
 *        bit to be set to 1
 * @param int32_t high that is the bit number of the highest numbered
 *        bit to be set to 1
 * @return an uint64_t that holds the modified source
 */
uint64_t Tools::setBits(uint64_t source, int32_t low, int32_t high)
{
	uint64_t num = 0;
	uint64_t mask = 1;
	if((low >= 0 && low <= 63) && (high >= 0 && high <= 63) && (low <= high))
	{
		int length = high-low + 1;
		if(length < 64)
		{
			mask = (mask << length)-1;
			mask = mask << low;
			num = mask | source;
		}
		else
		{
			num = -1;
		}
		//printf("S: %lx\n", source);
		//printf("M: %lx\n", mask);
		//printf("F: %lx\n", num);
	}
	else
	{
		num = source;
	}
   return num;
}

/* clearBits:
 * sets the bits of source in the range low to high to 0 (clears them) and
 * returns that value. returns source if the low or high
 * bit numbers are out of range
 *
 * for example, clearBits(0x1122334455667788, 0, 7) returns 0x1122334455667700
 *              clearBits(0x1122334455667788, 8, f) returns 0x1122334455660088
 *
 * @param uint64_t source 
 * @param int32_t low that is the bit number of the lowest numbered
 *        bit to be set to 0
 * @param int32_t high that is the bit number of the highest numbered
 *        bit to be set to 0
 * @return an uint64_t that holds the modified source
 */
uint64_t Tools::clearBits(uint64_t source, int32_t low, int32_t high)
{
   	uint64_t num = 0;
	uint64_t mask = 1;
	if((low >= 0 && low <= 63) && (high >= 0 && high <= 63) && (low <= high))
	{
		int length = high-low + 1;
		if(length < 64)
		{
			mask = (mask << length)-1;
			mask = mask << low;
			mask = ~mask;
			num = mask & source;
		}
		else
		{
			num = -1;
		}
		//printf("S: %lx\n", source);
		//printf("M: %lx\n", mask);
		//printf("F: %lx\n", num);
	}
	else
	{
		num = source;
	}
   return num;
}

/* copyBits:
 * copies length bits from the source to a destination and returns the
 * modified destination. If low bit number of the source or 
 * dest is out of range or the calculated source or dest high bit 
 * number is out of range, then the unmodified destination is returned.
 *
 * for example,
 *   copyBits(0x1122334455667788, 0x8877665544332211, 0, 0, 8) 
 *           returns 0x8877665544332288
 *   copyBits(0x1122334455667788, 0x8877665544332211, 0, 8, 8) 
 *           returns 0x8877665544338811
 *
 * @param uint64_t source 
 * @param uint64_t dest 
 * @param int32_t srclow that is the bit number of the lowest numbered
 *        bit of the source to be copied
 * @param int32_t destlow that is the bit number of the lowest numbered
 *        bit of the destination to be modified
 * @param int32_t length that is the number of bits to be copied
 * @return uint64_t that is the modifed dest
 */
uint64_t Tools::copyBits(uint64_t source, uint64_t dest, 
                         int32_t srclow, int32_t dstlow, int32_t length)
{
   	uint64_t mask = 0;
	if((srclow >= 0 && srclow <= 64)&&(dstlow >= 0 && dstlow <= 64)&&(length+srclow <= 64)&&(length+dstlow <= 64))
	{
		mask = Tools::getBits(source, srclow, srclow+length-1);
		mask = mask << dstlow;
		
		dest = Tools::clearBits(dest, dstlow, dstlow+length-1);
		
		dest = dest | mask;
	}
	
	//printf("S: %lx\n", source);
	//printf("M: %lx\n", mask);
	//printf("D: %lx\n", dest);
	return dest;
}

/* setByte:
 * sets the bits of source identfied by the byte number to 1 and
 * returns that value. if the byte number is out of range then source
 * is returned unchanged.
 *
 * for example, setByte(0x1122334455667788, 0) returns 0x11223344556677ff
 *              setByte(0x1122334455667788, 1) returns 0x112233445566ff88
 *              setByte(0x1122334455667788, 8) returns 0x1122334455667788
 *
 * @param uint64_t source 
 * @param int32_t byteNum that indicates the number of the byte to be
 *        set to 0xff; the low order byte is byte number 0
 * @return uint64_t that is source with byte byteNum set to 0xff
 */
uint64_t Tools::setByte(uint64_t source, int32_t byteNum)
{
   	uint64_t num = 0;
	uint64_t mask = 0xff;
	if(byteNum < 8)
	{
		mask = mask << (byteNum * 8);
		num = mask | source;
		
		//printf("B: %lx\n", byteNum);
		//printf("S: %lx\n", source);
		//printf("M: %lx\n", mask);
		//printf("F: %lx\n", num);
	}
	else
	{
		num = source;
	}
   return num;
}

/* sign:
 * assumes source contains a 64 bit two's complement value and
 * returns the sign (1 or 0)
 *
 * for example, sign(0xffffffffffffffff) returns 1
 *              sign(0x0000000000000000) returns 0
 *              sign(0x8000000000000000) returns 1
 *
 * @param uint64_t source
 * @return 1 if source is negative when treated as a two's complement 
 *         value and 0 otherwise
 */
uint8_t Tools::sign(uint64_t source)
{
   return source >> 63;
}

/* addOverflow:
 * assumes that op1 and op2 contain 64 bit two's complement values
 * and returns true if an overflow would occur if they are summed
 * and false otherwise
 *
 * for example, addOverflow(0x8000000000000000, 0x8000000000000000) returns 1
 *              addOverflow(0x7fffffffffffffff, 0x7fffffffffffffff) returns 1
 *              addOverflow(0x8000000000000000, 0x7fffffffffffffff) returns 0
 *
 * @param uint64_t op1 that is one of the operands of the addition
 * @param uint64_t op2 that is the other operand of the addition
 * @return true if op1 + op2 would result in an overflow assuming that op1
 *         and op2 contain 64-bit two's complement values
 */
bool Tools::addOverflow(uint64_t op1, uint64_t op2)
{
	bool test = false;
	uint8_t fBit = Tools::sign(op1);
	uint8_t sBit = Tools::sign(op2);
	uint8_t tBit = Tools::sign(op1+op2);
	
	//printf("F:%d\n",fBit);
	//printf("S:%d\n",sBit);
	//printf("T:%d\n",tBit);
	
	if((!fBit && !sBit && tBit)||(fBit && sBit && !tBit))
	{
		test = true;
	}
	else if((fBit && !sBit && tBit)||(!fBit && sBit && tBit))
	{
		test = false;
	}

   return test;
}

/* subOverflow:
 * assumes that op1 and op2 contain 64 bit two's complement values
 * and returns true if an overflow would occur from op2 - op1
 * and false otherwise
 *
 * for example, subOverflow(0x8000000000000000, 0x8000000000000000) returns 0
 *              subOverflow(0x7fffffffffffffff, 0x7fffffffffffffff) returns 0
 *              subOverflow(0x8000000000000000, 0x7fffffffffffffff) returns 1
 *
 * @param uint64_t op1 that is one of the operands of the subtraction
 * @param uint64_t op2 that is the other operand of the subtraction
 * @return true if op2 - op1 would result in an overflow assuming that op1
 *         and op2 contain 64-bit two's complement values
 */
bool Tools::subOverflow(uint64_t op1, uint64_t op2)
{
	return addOverflow(-op1,op2);
}
