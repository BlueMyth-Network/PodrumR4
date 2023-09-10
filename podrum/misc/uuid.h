#ifndef PODRUM_MISC_UUID_H
#define PODRUM_MISC_UUID_H

#include <string.h>
#include <stdint.h>

unsigned char *uuid_str_to_bin(char *uuid_str, int str_format);

char* uuid_bin_to_str(unsigned char *uuid_bin, int str_format);

#endif