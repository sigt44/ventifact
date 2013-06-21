#ifndef FLAG_H
#define FLAG_H

#define NO_FLAGS 0

void flag_On(unsigned int *flag, unsigned int bitMask);

void flag_Off(unsigned int *flag, unsigned int bitMask);

void flag_Toggle(unsigned int *flag, unsigned int bitMask);

int flag_Check(unsigned int *flag, unsigned int bitMask);

void flag_Clear(unsigned int *flag);

#endif
