#include <podrum/misc/uuid.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

unsigned char * uuid_str_to_bin(char *uuid_str, int str_format)
{
    unsigned char *uuid_bytes = malloc(16 * sizeof(unsigned char *));
	if (str_format != 1){
		sscanf(uuid_str, 
				"%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
				&uuid_bytes[0], &uuid_bytes[1], &uuid_bytes[2], &uuid_bytes[3],
				&uuid_bytes[4], &uuid_bytes[5], &uuid_bytes[6], &uuid_bytes[7],
				&uuid_bytes[8], &uuid_bytes[9], &uuid_bytes[10], &uuid_bytes[11],
				&uuid_bytes[12], &uuid_bytes[13], &uuid_bytes[14], &uuid_bytes[15]
		);
	}else{
		sscanf(uuid_str, 
				"%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
				&uuid_bytes[8], &uuid_bytes[7], &uuid_bytes[6], &uuid_bytes[5],
				&uuid_bytes[4], &uuid_bytes[3], &uuid_bytes[2], &uuid_bytes[1],
				&uuid_bytes[0], &uuid_bytes[15], &uuid_bytes[14], &uuid_bytes[13],
				&uuid_bytes[12], &uuid_bytes[11], &uuid_bytes[10], &uuid_bytes[9]
		);
	}
	return uuid_bytes;
}
/*sprintf(uuid_str, 
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", 
        uuid_bytes[0], uuid_bytes[1], uuid_bytes[2], uuid_bytes[3],
        uuid_bytes[4], uuid_bytes[5], uuid_bytes[6], uuid_bytes[7],
        uuid_bytes[8], uuid_bytes[9], uuid_bytes[10], uuid_bytes[11],
        uuid_bytes[12], uuid_bytes[13], uuid_bytes[14], uuid_bytes[15]);*/

char* uuid_bin_to_str(unsigned char *uuid_bin, int str_format)
{
	const char uuid_char_order[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	const char custom_char_order[16] = {8, 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9};
	const char *char_order;
	if (str_format != 1)
		char_order = uuid_char_order;
	else
		char_order = custom_char_order;
    char *uuid_str = malloc(37 * sizeof(char *));
    int i;
	for (i = 0; i < 16; i++) {
		sprintf(uuid_str, "%02x", uuid_bin[char_order[i]]);
		uuid_str += 2;
		switch (i) {
		case 3:
		case 5:
		case 7:
		case 9:
			*uuid_str++ = '-';
			break;
		}
	}
    return uuid_str;
}
