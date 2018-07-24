/*
 *	$Id$
 *	MD5 class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2009, IRIT UPS.
 *
 *	OTAWA is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	OTAWA is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OTAWA; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * This MD5 implementation is a derivative of http://crazyjoke.free.fr/md5/
 * delivered under GPL licence.
 */

#include <elm/checksum/MD5.h>
#include <elm/util/MessageException.h>
#include <elm/io/BlockInStream.h>
#include <elm/compare.h>

#define MD5_BUFFER 1024

// Basic md5 functions
#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (~z & y))
#define H(x,y,z) (x ^ y ^ z)
#define I(x,y,z) (y ^ (x | ~z))

// Rotate left 32 bits values (words)
#define ROTATE_LEFT(w,s) ((w << s) | ((w & 0xFFFFFFFF) >> (32 - s)))
#define FF(a,b,c,d,x,s,t) (a = b + ROTATE_LEFT((a + F(b,c,d) + x + t), s))
#define GG(a,b,c,d,x,s,t) (a = b + ROTATE_LEFT((a + G(b,c,d) + x + t), s))
#define HH(a,b,c,d,x,s,t) (a = b + ROTATE_LEFT((a + H(b,c,d) + x + t), s))
#define II(a,b,c,d,x,s,t) (a = b + ROTATE_LEFT((a + I(b,c,d) + x + t), s))

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

#define GET_UINT32(a,b,i)				\
{							\
	(a) = ( (t::uint32) (b)[(i)	]      )	\
	    | ( (t::uint32) (b)[(i)+1] << 8 )	\
	    | ( (t::uint32) (b)[(i)+2] << 16)	\
	    | ( (t::uint32) (b)[(i)+3] << 24);	\
}

static unsigned char MD5_PADDING [64] = { /* 512 Bits */
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


namespace elm { namespace checksum {

/**
 * @class MD5
 * Apply the MD5 algorithm (http://fr.wikipedia.org/wiki/MD5)
 * to compute a checksum.
 *
 * Data are checksummed calling any one of the @ref put() methods
 * (or using the overloaded operator <<).
 * When all data has been checksummed, the checksum value is returned by
 * digest().
 */


/**
 * Build a new MD5 checksum builder.
 */
MD5::MD5(void): finalized(false) {
	buf = new unsigned char[MD5_BUFFER];
	memset(buf, '\0', MD5_BUFFER);
	size = 0;
	bits = 0;

	regs.A = 0x67452301;
	regs.B = 0xefcdab89;
	regs.C = 0x98badcfe;
	regs.D = 0x10325476;
}


/**
 */
MD5::~MD5(void) {
	if(buf)
		delete [] buf;
}


/**
 * Put a data block in the checksum.
 * @param block		Block address.
 * @param length	Block length.
 */
void MD5::put(const void *block, t::uint32 length) {
	while(length + size > MD5_BUFFER) {
		memcpy(buf + size, block, min(length, MD5_BUFFER - size));
		update();
	}
	memcpy(buf + size, block, length);
	size += length;
}


/**
 */
void MD5::update(void) {
	unsigned char buffer[64]; // 512 bits
	int i;

	ASSERTP(!finalized, "already finalized MD5");

	for(i = 0; size - i > 63; i += 64) {
		memcpy (buffer, buf + i, 64);
		encode(buffer);
		bits += 64;
	}
	memcpy(buffer, buf + i, size - i);
	memcpy(buf, buffer, size - i);
	size -= i;
}


/**
 */
void MD5::encode(unsigned char *buffer) {
	t::uint32	a = regs.A,
				b = regs.B,
				c = regs.C,
				d = regs.D;
	t::uint32	x[16];

	GET_UINT32 (x[ 0],buffer, 0);
	GET_UINT32 (x[ 1],buffer, 4);
	GET_UINT32 (x[ 2],buffer, 8);
	GET_UINT32 (x[ 3],buffer,12);
	GET_UINT32 (x[ 4],buffer,16);
	GET_UINT32 (x[ 5],buffer,20);
	GET_UINT32 (x[ 6],buffer,24);
	GET_UINT32 (x[ 7],buffer,28);
	GET_UINT32 (x[ 8],buffer,32);
	GET_UINT32 (x[ 9],buffer,36);
	GET_UINT32 (x[10],buffer,40);
	GET_UINT32 (x[11],buffer,44);
	GET_UINT32 (x[12],buffer,48);
	GET_UINT32 (x[13],buffer,52);
	GET_UINT32 (x[14],buffer,56);
	GET_UINT32 (x[15],buffer,60);

	/* Round 1 */
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */

	GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	regs.A += a;
	regs.B += b;
	regs.C += c;
	regs.D += d;
}


void MD5::finalize(void) {
	unsigned char buffer [64]; // 512 bits
	int i;

	// finish the block
	if(size + 1 > 56) { // We have to create another block

		// finalizing current block
		memcpy(buffer, buf, size);
		memcpy(buffer + size, MD5_PADDING, 64 - size);
		encode(buffer);
		bits += size;
		size = 0;

		// Proceed final block
		memset(buffer, '\0', 56);
		addsize(buffer, 56, bits);
		encode(buffer);
	}
	else {
		memcpy(buffer, buf, size);
		bits += size;
		memcpy(buffer + size, MD5_PADDING, 56 - size);
		addsize(buffer, 56, bits);
		encode(buffer);
	}

	// update digest
	for(i = 0; i < 4; i++)
		_digest[i] = (unsigned char)((regs.A >> (i * 8)) & 0xFF);
	for(; i < 8; i++)
		_digest[i] = (unsigned char)((regs.B >> ((i - 4) * 8)) & 0xFF);
	for(; i < 12; i++)
		_digest[i] = (unsigned char)((regs.C >> ((i - 8) * 8)) & 0xFF);
	for(; i < 16; i++)
		_digest[i] = (unsigned char)((regs.D >> ((i - 12) * 8)) & 0xFF);
	finalized = true;
}


/**
 */
void MD5::addsize (unsigned char *M, md5_size index, md5_size oldlen) {
	ASSERT(((index * 8) % 512) == 448); // If padding is not done then exit */
	M[index++] = (unsigned char) ((oldlen << 3) & 0xFF);
	M[index++] = (unsigned char) ((oldlen >> 5) & 0xFF);
	M[index++] = (unsigned char) ((oldlen >> 13) & 0xFF);
	M[index++] = (unsigned char) ((oldlen >> 21) & 0xFF);
	// Fill with 0 because md5_size is 32 bits long
	M[index++] = 0; M[index++] = 0;
	M[index++] = 0; M[index++] = 0;
}


/**
 * Get the check as 512 bits byte array.
 * @param tab	Digest result.
 */
void MD5::digest(digest_t tab) {
	if(!finalized)
		finalize();
	memcpy(tab, _digest, sizeof(digest_t));
}


/**
 * Print the MD5 digest as an hexadecimal value.
 */
void MD5::print(io::Output& out) {
	if(!finalized)
		finalize();
	for(size_t i = 0; i < sizeof(digest_t); i++)
		out << io::right(io::pad('0', io::width(2, io::hex(_digest[i]))));
}


/**
 * Put a C string in the checksum.
 * @param str	C string to put in.
 */
void MD5::put(const CString& str) {
	put(str.chars(), str.length());
}


/**
 * Put a string in the checksum.
 * @param str	String to put in.
 */
void MD5::put(const String& str) {
	put(&str, str.length());
}


/**
 * Put an input stream in the buffer.
 * Read to the end of the stream.
 * @param in	Input stream to read.
 * @throw io::IOException	If there is an error during stream read.
 */
void MD5::put(io::InStream& in) {
	do {
		int r = in.read(buf + size, MD5_BUFFER - size);
		if(r < 0)
			throw io::IOException("MD5Sum: error during stream read");
		size += r;
		//cout << size << " " << r << io::endl;
		update();
	} while(size == 0);
}


/**
 * Put at most length bytes from the given stream
 * to compute the MD5 checksum.
 * @param in		Input stream to read.
 * @param length	Maximum number of bytes to read.
 * @throw io::IOException	If there is an error during stream read.
 */
void MD5::put(io::InStream& in, int length) {
	t::uint32 l = length;
	do {
		int r = in.read(buf + size, min(l, MD5_BUFFER - size));
		if(r < 0)
			throw io::IOException("MD5Sum: error during stream read");
		size += r;
		l -= r;
		update();
	} while(size == 0 && l != 0);
}


/**
 */
int MD5::write(const char *buffer, int size) {
	put(buffer, size);
	return size;
}


/**
 */
int MD5::flush(void) {
	return 0;
}


/**
 */
cstring MD5::lastErrorMessage(void) {
	return "";
}

} } // elm::checksum
