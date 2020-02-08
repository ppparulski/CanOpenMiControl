#pragma once
#include "stddef.h"

inline void DataCopy(uint8_t* dest, uint8_t* source, uint16_t size)
{
	for(size_t i = 0; i < size; i++)
		dest[i] = source[i];
}
