#include <podrum/misc/base64.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

unsigned char * char_base64_encode(const unsigned char *src, size_t len, size_t *out_len)
{
	unsigned char *out, *pos;
	const unsigned char *end, *in;
	size_t olen;
	int line_len;

	olen = len * 4 / 3 + 4; /* 3-byte blocks to 4-byte */
	olen += olen / 72; /* line feeds */
	olen++; /* nul termination */
	if (olen < len)
		return NULL; /* integer overflow */
	out = malloc(olen);
	if (out == NULL)
		return NULL;
	end = src + len;
	in = src;
	pos = out;
	line_len = 0;
	while (end - in >= 3) {
		*pos++ = BASE64_TABLE[in[0] >> 2];
		*pos++ = BASE64_TABLE[((in[0] & 0x03) << 4) | (in[1] >> 4)];
		*pos++ = BASE64_TABLE[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
		*pos++ = BASE64_TABLE[in[2] & 0x3f];
		in += 3;
		line_len += 4;
		if (line_len >= 72) {
			// *pos++ = '\n';
			line_len = 0;
		}
	}

	if (end - in) {
		*pos++ = BASE64_TABLE[in[0] >> 2];
		if (end - in == 1) {
			*pos++ = BASE64_TABLE[(in[0] & 0x03) << 4];
			*pos++ = '=';
		} else {
			*pos++ = BASE64_TABLE[((in[0] & 0x03) << 4) |
					      (in[1] >> 4)];
			*pos++ = BASE64_TABLE[(in[1] & 0x0f) << 2];
		}
		*pos++ = '=';
		line_len += 4;
	}

	*pos = '\0';
	if (out_len)
		*out_len = pos - out;
	return out;
}

unsigned char * char_base64_decode(const unsigned char *src, size_t len, size_t *out_len)
{
	unsigned char dtable[256], *out, *pos, block[4], tmp;
	size_t i, count, olen;
	int pad = 0;

	memset(dtable, 0x80, 256);
	for (i = 0; i < sizeof(BASE64_TABLE) - 1; i++)
		dtable[BASE64_TABLE[i]] = (unsigned char) i;
	dtable['='] = 0;

	count = 0;
	for (i = 0; i < len; i++) {
		if (dtable[src[i]] != 0x80)
			count++;
	}

	if (count == 0 || count % 4)
		return NULL;

	olen = count / 4 * 3;
	pos = out = malloc(olen);
	if (out == NULL)
		return NULL;

	count = 0;
	for (i = 0; i < len; i++) {
		tmp = dtable[src[i]];
		if (tmp == 0x80)
			continue;

		if (src[i] == '=')
			pad++;
		block[count] = tmp;
		count++;
		if (count == 4) {
			*pos++ = (block[0] << 2) | (block[1] >> 4);
			*pos++ = (block[1] << 4) | (block[2] >> 2);
			*pos++ = (block[2] << 6) | block[3];
			count = 0;
			if (pad) {
				if (pad == 1)
					pos--;
				else if (pad == 2)
					pos -= 2;
				else {
					/* Invalid padding */
					free(out);
					return NULL;
				}
				break;
			}
		}
	}

	*out_len = pos - out;
	return out;
}

unsigned char * char_base64_url_encode(const unsigned char *src, size_t len, size_t *out_len)
{
	unsigned char *out, *pos;
	const unsigned char *end, *in;
	size_t olen;
	int line_len;

	olen = len * 4 / 3 + 4; /* 3-byte blocks to 4-byte */
	olen += olen / 72; /* line feeds */
	olen++; /* nul termination */
	if (olen < len)
		return NULL; /* integer overflow */
	out = malloc(olen);
	if (out == NULL)
		return NULL;
	end = src + len;
	in = src;
	pos = out;
	line_len = 0;
	while (end - in >= 3) {
		*pos++ = BASE64_URL_TABLE[in[0] >> 2];
		*pos++ = BASE64_URL_TABLE[((in[0] & 0x03) << 4) | (in[1] >> 4)];
		*pos++ = BASE64_URL_TABLE[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
		*pos++ = BASE64_URL_TABLE[in[2] & 0x3f];
		in += 3;
		line_len += 4;
		if (line_len >= 72) {
			line_len = 0;
		}
	}

	if (end - in) {
		*pos++ = BASE64_URL_TABLE[in[0] >> 2];
		if (end - in == 1) {
			*pos++ = BASE64_URL_TABLE[(in[0] & 0x03) << 4];
		} else {
			*pos++ = BASE64_URL_TABLE[((in[0] & 0x03) << 4) |
					      (in[1] >> 4)];
			*pos++ = BASE64_URL_TABLE[(in[1] & 0x0f) << 2];
		}
		line_len += 4;
	}

	*pos = '\0';
	if (out_len)
		*out_len = pos - out;
	return out;
}

unsigned char * char_base64_url_decode(const unsigned char *src, size_t len, size_t *out_len)
{
	unsigned char dtable[256], *out, *pos, block[4], tmp;
	size_t i, count, olen;
	int pad = 0;

	memset(dtable, 0x80, 256);
	for (i = 0; i < sizeof(BASE64_URL_TABLE) - 1; i++)
		dtable[BASE64_URL_TABLE[i]] = (unsigned char) i;
	dtable['='] = 0;

	count = 0;
	for (i = 0; i < len; i++) {
		if (dtable[src[i]] != 0x80)
			count++;
	}

	if (count == 0 || count % 4)
		return NULL;

	olen = count / 4 * 3;
	pos = out = malloc(olen);
	if (out == NULL)
		return NULL;

	count = 0;
	for (i = 0; i < len; i++) {
		tmp = dtable[src[i]];
		block[count] = tmp;
		count++;
		if (count == 4) {
			*pos++ = (block[0] << 2) | (block[1] >> 4);
			*pos++ = (block[1] << 4) | (block[2] >> 2);
			*pos++ = (block[2] << 6) | block[3];
			count = 0;
			if (pad) {
				if (pad == 1)
					pos--;
				else if (pad == 2)
					pos -= 2;
				else {
					/* Invalid padding */
					free(out);
					return NULL;
				}
				break;
			}
		}
	}

	*out_len = pos - out;
	return out;
}

binary_stream_t base64_decode(char *base64_string)
{
	binary_stream_t stream;
	stream.buffer = (int8_t *) malloc(0);
	stream.offset = 0;
	stream.size = 0;
	size_t len = strlen(base64_string);
	size_t i;
	uint8_t padding = 0;
	for (i = 0; i < len; i += 4) {
		uint8_t i1 = 0;
		uint8_t i2 = 0;
		uint8_t i3 = 0;
		uint8_t i4 = 0;
		char c1 = 0;
		char c2 = 0;
		char c3 = 0;
		char c4 = 0;
		if (i < len) {
			if (base64_string[i] == '=') {
				++padding;
			} else {
				c1 = base64_string[i];
			}
		} else {
			break;
		}
		if ((i + 1) < len) {
			if (base64_string[i + 1] == '=') {
				++padding;
			} else {
				c2 = base64_string[i + 1];
			}
		} else {
			break;
		}
		if ((i + 2) < len) {
			if (base64_string[i + 2] == '=') {
				++padding;
			} else {
				c3 = base64_string[i + 2];
			}
		} else {
			++padding;
		}
		if ((i + 3) < len) {
			if (base64_string[i + 3] == '=') {
				++padding;
			} else {
				c4 = base64_string[i + 3];
			}
		} else {
			++padding;
		}
		if (padding > 2) {
			break;
		}
		int step = 0;
		uint8_t ii;
		for (ii = 0; ii < 64; ++ii) {
			if (BASE64_TABLE[ii] == c1) {
				i1 = ii;
				++step;
			}
			if (BASE64_TABLE[ii] == c2) {
				i2 = ii;
				++step;
			}
			if (BASE64_TABLE[ii] == c3) {
				i3 = ii;
				++step;
			}
			if (BASE64_TABLE[ii] == c4) {
				i4 = ii;
				++step;
			}
			if (step == 4) {
				break;
			}
		}
		uint32_t bit_array = (i1 << 18) | (i2 << 12) | (i3 << 6) | i4;
		put_unsigned_byte((bit_array >> 16) & 0xff, &stream);
		if (padding < 2) {
			put_unsigned_byte((bit_array >> 8) & 0xff, &stream);
		}
		if (padding < 1) {
			put_unsigned_byte(bit_array & 0xff, &stream);
		}
	}
	return stream;
}

char *base64_encode(binary_stream_t stream)
{
	char *out = (char *) malloc(0);
	size_t size = 0;
	size_t i;
	uint8_t padding = 0;
	for (i = 0; i < stream.size; i += 3) {
		uint8_t b1 = 0;
		uint8_t b2 = 0;
		uint8_t b3 = 0;
		if (i < stream.size) {
			b1 = stream.buffer[i];
		}
		if ((i + 1) < stream.size) {
			b2 = stream.buffer[i + 1];
		} else {
			++padding;
		}
		if ((i + 2) < stream.size) {
			b3 = stream.buffer[i + 2];
		} else {
			++padding;
		}
		uint32_t bit_array = (b1 << 16) | (b2 << 8) | b3;
		uint8_t i1 = (bit_array >> 18) & 0x3f;
		uint8_t i2 = (bit_array >> 12) & 0x3f;
		uint8_t i3 = (bit_array >> 6) & 0x3f;
		uint8_t i4 = bit_array & 0x3f;
		size += 4;
		out = realloc(out, size);
		out[size - 4] = BASE64_TABLE[i1];
		out[size - 3] = BASE64_TABLE[i2];
		if (padding < 2) {
			out[size - 2] = BASE64_TABLE[i3];
		} else {
			out[size - 2] = '=';
		}
		if (padding < 1) {
			out[size - 1] = BASE64_TABLE[i4];
		} else {
			out[size - 1] = '=';
		}
	}
	++size;
	out = realloc(out, size);
	out[size - 1] = 0;
	return out;
}

char *base64_url_encode(binary_stream_t stream) {
    char *encoded = base64_encode(stream);
    size_t len = strlen(encoded);
    for (size_t i = 0; i < len; i++) {
        if (encoded[i] == '+') {
            encoded[i] = '-';
        } else if (encoded[i] == '/') {
            encoded[i] = '_';
        }
    }
    return encoded;
}

binary_stream_t base64_url_decode(char *base64_string) {
    size_t len = strlen(base64_string);
    char *temp = (char*)malloc(len + 1);
    memcpy(temp, base64_string, len + 1);
    for (size_t i = 0; i < len; ++i) {
        if (temp[i] == '-') {
            temp[i] = '+';
        } else if (temp[i] == '_') {
            temp[i] = '/';
        }
    }
    binary_stream_t decoded = base64_decode(temp);
    free(temp);
    return decoded;
}