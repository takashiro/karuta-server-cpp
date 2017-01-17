#include "util.h"

#include <iostream>
#include <memory.h>
#include <stdlib.h>

KA_NAMESPACE_BEGIN

std::string base64_encode(const uchar *data, uint64 length)
{
	const char *index_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::string result;
	uint64 i, max;
	for (i = 2, max = length; i < max; i += 3) {
		result += index_table[data[i - 2] >> 2];
		result += index_table[((data[i - 2] & 0x3) << 4) | ((data[i - 1] & 0xF0) >> 4)];
		result += index_table[((data[i - 1] & 0xF) << 2) | ((data[i] & 0xC0) >> 6)];
		result += index_table[data[i] & 0x3F];
	}

	i -= 2;
	if (i + 1 < max) {
		result += index_table[data[i] >> 2];
		result += index_table[((data[i] & 0x3) << 4) | ((data[i + 1] & 0xF0) >> 4)];
		result += index_table[(data[i + 1] & 0xF) << 2];
		result += '=';
	}
	else if (i < max) {
		result += index_table[data[i] >> 2];
		result += index_table[(data[i] & 0x3) << 4];
		result += '=';
		result += '=';
	}
	return result;
}

std::string base64_encode(const std::string &str)
{
	return base64_encode(reinterpret_cast<const unsigned char *>(str.data()), str.size());
}

void sha1_chunk(uint *h, uint *words)
{
	uint a = h[0];
	uint b = h[1];
	uint c = h[2];
	uint d = h[3];
	uint e = h[4];

	uint f, k;
	int i;

	for (i = 16; i < 80; i++) {
		words[i] = words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16];
		words[i] = (words[i] << 1) | words[i] >> 31;
	}

	for (i = 0; i < 80; i++) {
		if (i < 20) {
			f = (b & c) | (~b & d);
			k = 0x5A827999;
		}
		else if (i < 40) {
			f = b ^ c ^ d;
			k = 0x6ED9EBA1;
		}
		else if (i  < 60) {
			f = (b & c) | (b & d) | (c & d);
			k = 0x8F1BBCDC;
		}
		else {
			f = b ^ c ^ d;
			k = 0xCA62C1D6;
		}

		unsigned int tmp = (a << 5 | (a >> 27)) + f + e + k + words[i];
		e = d;
		d = c;
		c = (b << 30) | (b >> 2);
		b = a;
		a = tmp;
	}

	h[0] += a;
	h[1] += b;
	h[2] += c;
	h[3] += d;
	h[4] += e;
}

std::string sha1(const char *data, uint64 length)
{
	uint h[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
	uint words[80];
	int i = 0;

	//For each 512-bit (64-byte) chunk
	const char *chunk = data;
	uint64 end;
	for (end = 64; end <= length; end += 64) {
		const char *w = chunk;
		for (i = 0; i < 16; i++) {
			words[i] = (w[0] << 24) | (w[1] << 16) | (w[2] << 8) | w[3];
			w += 4;
		}
		sha1_chunk(h, words);
		chunk += 64;
	}

	//Fill the last chunk
	if (end > length) {
		end -= 64;
		if (end <= length) {
			end += 4;
			for (; end <= length; end += 4) {
				words[i] = (chunk[0] << 24) | (chunk[1] << 16) | (chunk[2] << 8) | chunk[3];
				i++;
				chunk += 4;
			}
			end -= 4;
		}
	}

	if (end > length) {
		end -= 4;
		int j = 0;
		for (; end < length; end++) {
			words[i] = (words[i] << 8) | chunk[j];
			j++;
		}
		words[i] = (words[i] << 8) | 0x80;
		j++;
		for (; j < 4; j++) {
			words[i] <<= 8;
		}
	}
	else {
		words[i] = 0x80000000;
	}

	if (i == 15) {
		sha1_chunk(h, words);
		i = -1;
	}

	for (i++; i < 14; i++) {
		words[i] = 0;
	}

	uint64 bits = length * 8;
	const uchar *from = reinterpret_cast<const uchar *>(&bits);
	words[14] = (from[7] << 24) | (from[6] << 16) | (from[5] << 8) | from[4];
	words[15] = (from[3] << 24) | (from[2] << 16) | (from[1] << 8) | from[0];

	sha1_chunk(h, words);

	std::string result;
	result.reserve(20);
	for (uint num : h) {
		const char *w = reinterpret_cast<const char *>(&num);
		result.push_back(w[3]);
		result.push_back(w[2]);
		result.push_back(w[1]);
		result.push_back(w[0]);
	}
	return result;
}

std::string sha1(const std::string &str)
{
	return sha1(str.data(), str.size());
}

KA_NAMESPACE_END
