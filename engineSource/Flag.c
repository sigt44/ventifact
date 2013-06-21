#include "Flag.h"

void flag_On(unsigned int *flag,unsigned int bitMask)
{
	*flag |= bitMask;

	return;
}

void flag_Off(unsigned int *flag,unsigned int bitMask)
{
	*flag = *flag & ~(bitMask);

	return;
}

void flag_Toggle(unsigned int *flag,unsigned int bitMask)
{
	*flag = *flag ^ bitMask;
	return;
}

int flag_Check(unsigned int *flag, unsigned int bitMask)
{
	if((*flag & bitMask) == bitMask)
		return 1;

	return 0;
}

void flag_Clear(unsigned int *flag)
{
	*flag = NO_FLAGS;
}



