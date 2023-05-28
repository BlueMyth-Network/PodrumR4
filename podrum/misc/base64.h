

#ifndef PODRUM_MISC_BASE64_H
#define PODRUM_MISC_BASE64_H

#include <stdlib.h>
#include <cbinarystream/binary_stream.h>

#define BASE64_TABLE "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
#define BASE64_URL_TABLE "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"

unsigned char * char_base64_encode(const unsigned char *src, size_t len, size_t *out_len);

unsigned char * char_base64_decode(const unsigned char *src, size_t len, size_t *out_len);

unsigned char * char_base64_url_encode(const unsigned char *src, size_t len, size_t *out_len);

unsigned char * char_base64_url_decode(const unsigned char *src, size_t len, size_t *out_len);

binary_stream_t base64_decode(char *base64_string);

char *base64_encode(binary_stream_t stream);

binary_stream_t base64_url_decode(char *base64_string);

char *base64_url_encode(binary_stream_t stream);

#endif