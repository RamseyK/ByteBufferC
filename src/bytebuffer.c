/**
   ByteBuffer (C implementation)
   bytebuffer.c
   Copyright 2011-2013 Ramsey Kant

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "bytebuffer.h"

// Wrap around an existing buf - will not copy buf
byte_buffer *bb_new_wrap(uint8_t *buf, size_t len) {
	byte_buffer *bb = (byte_buffer*)malloc(sizeof(byte_buffer));
	bb->pos = 0;
	bb->wrapped = true;
	bb->len = len;
	bb->buf = buf;
	return bb;
}

// Copy len bytes from buf into the newly created byte buffer
byte_buffer *bb_new_copy(uint8_t *buf, size_t len) {
	byte_buffer *bb = (byte_buffer*)malloc(sizeof(byte_buffer));
	bb->pos = 0;
	bb->wrapped = false;
	bb->len = len;
	bb->buf = (uint8_t*)malloc(len);
	memcpy(bb->buf, buf, len);
	return bb;
}

byte_buffer *bb_new(size_t len) {
	byte_buffer *bb = (byte_buffer*)malloc(sizeof(byte_buffer));
	bb->pos = 0;
	bb->wrapped = false;
	bb->len = len;
	bb->buf = (uint8_t*)calloc(len, sizeof(uint8_t));
	return bb;
}

byte_buffer *bb_new_from_file(const char *path, const char *fopen_opts) {
	struct stat sb;
	FILE *fp;
	size_t bytes_read = 0;
	uint8_t *file_buf = NULL;
	byte_buffer *bb = NULL;

	// Get the size of the file
	if(stat(path, &sb) != 0) {
		printf("Could not get the size of the file %s\n", path);
		return NULL;
	}

	// Open file as read only
	fp = fopen(path, fopen_opts);
	if(fp == NULL) {
		printf("Could not open file %s\n", path);
		return NULL;
	}

	// Read data into a memory buffer
	file_buf = (uint8_t*)malloc(sb.st_size);
	bytes_read = fread(file_buf, sizeof(uint8_t), sb.st_size, fp);

	fclose(fp);

	if(bytes_read != sb.st_size) {
		printf("Incomplete read. Read %i out of %i bytes.\n", (int)bytes_read, (int)sb.st_size);
		free(file_buf);
		return NULL;
	}

	bb = bb_new_wrap(file_buf, bytes_read);

	return bb;
}

byte_buffer *bb_new_default() {
	byte_buffer *bb = (byte_buffer*)malloc(sizeof(byte_buffer));
	bb->pos = 0;
	bb->wrapped = false;
	bb->len = BB_DEFAULT_SIZE;
	bb->buf = (uint8_t*)calloc(BB_DEFAULT_SIZE, sizeof(uint8_t));
	return bb;
}

/*
 * Resizes the internal buffer and copys of the new_len worth of data from the old buffer.
 * Resizing will only work on buffers managed by byte_buffer (ie. not wrapped buffers)
 * This will also reset the read/write position.
 *
 * @return True if the resize was a success
 */
bool bb_resize(byte_buffer* bb, size_t new_len) {
	// Can't resize an internal buffer that may be used elsewhere
	if(bb->wrapped)
		return false;

	// Copy as much data from the old buffer as we can
	size_t copy_len = (new_len >= bb->len ? bb->len : new_len);
	uint8_t *old_buf = bb->buf;

	bb->pos = 0;
	bb->len = new_len;
	bb->buf = (uint8_t*)calloc(new_len, sizeof(uint8_t));
	memcpy(bb->buf, old_buf, copy_len);
	free(old_buf);

	return true;
}

void bb_free(byte_buffer *bb) {
	if(!bb->wrapped)
		free(bb->buf);

	free(bb);
}

void bb_skip(byte_buffer *bb, size_t len) {
	bb->pos += len;
}

// Number of bytes from the current read position till the end of the buffer
size_t bb_bytes_left(byte_buffer *bb) {
	return bb->len - bb->pos;
}

// Blank out the buffer and reset the position
void bb_clear(byte_buffer *bb) {
	memset(bb->buf, 0, bb->len);
}

// Return a new instance of a bytebuffer with the exact same contents and the same state
byte_buffer *bb_clone(byte_buffer *bb) {
	byte_buffer *ret = bb_new_copy(bb->buf, bb->len);
	ret->pos = bb->pos;
	return ret;
}

// Compare if the contents are equivalent
bool bb_equals(byte_buffer* bb1, byte_buffer* bb2) {
	if(bb1->len != bb2->len)
		return false;

	for(uint32_t i = 0; i < bb1->len; i++) {
		if(bb1->buf[i] != bb2->buf[i])
			return false;
	}

	return true;
}

void bb_replace(byte_buffer *bb, uint8_t key, uint8_t rep, uint32_t start, bool firstOccuranceOnly) {
	for(uint32_t i = start; i < bb->len; i++) {
		if(bb->buf[i] == key) {
			bb->buf[i] = rep;

			if(firstOccuranceOnly)
				return;
		}
	}
}

void bb_print_ascii(byte_buffer *bb) {
	for(uint32_t i = 0; i < bb->len; i++) {
		printf("%c ", bb->buf[i]);
	}
	printf("\n");
}

void bb_print_hex(byte_buffer *bb) {
	for(uint32_t i = 0; i < bb->len; i++) {
		printf("0x%02x ", bb->buf[i]);
	}
	printf("\n");
}

// Relative peek. Reads and returns the next byte in the buffer from the current position but does not increment the read position
uint8_t bb_peek(byte_buffer *bb) {
	//return *(uint8_t*)(bb->buf+bb->pos);
	return bb->buf[bb->pos];
}

// Relative get method. Reads the byte at the buffers current position then increments the position
uint8_t bb_get(byte_buffer *bb) {
	return bb->buf[bb->pos++];
}

// Absolute get method. Read byte at index
uint8_t bb_get_at(byte_buffer *bb, uint32_t index) {
	return bb->buf[index];
}

void bb_get_bytes_in(byte_buffer *bb, uint8_t *dest, size_t len) {
	for(size_t i = 0; i < len; i++) {
		dest[i] = bb_get(bb);
	}
}

void bb_get_bytes_at_in(byte_buffer *bb, uint32_t index, uint8_t *dest, size_t len) {
	for(size_t i = 0; i < len; i++) {
		dest[i] = bb_get_at(bb, index+i);
	}
}

// Return a new byte array of size len with the contents from the current position
uint8_t *bb_get_bytes(byte_buffer *bb, size_t len) {
	uint8_t *ret = (uint8_t*)malloc(len);
	memcpy(ret, bb->buf+bb->pos, len);
	bb->pos += len;
	return ret;
}

// Return a new byte array of size len with the contents from the index position
uint8_t *bb_get_bytes_at(byte_buffer *bb, size_t len, uint32_t index) {
	return (uint8_t*)malloc(len);
}

double bb_get_double(byte_buffer *bb) {
	double ret = *(double*)(bb->buf+bb->pos);
	bb->pos += sizeof(double);
	return ret;
}

double bb_get_double_at(byte_buffer *bb, uint32_t index) {
	return *(double*)(bb->buf+index);
}

float bb_get_float(byte_buffer *bb) {
	float ret = *(float*)(bb->buf+bb->pos);
	bb->pos += sizeof(float);
	return ret;
}

float bb_get_float_at(byte_buffer *bb, uint32_t index) {
	return *(float*)(bb->buf+index);
}

uint32_t bb_get_int(byte_buffer *bb) {
	uint32_t ret = *(uint32_t*)(bb->buf+bb->pos);
	bb->pos += sizeof(uint32_t);
	return ret;
}

uint32_t bb_get_int_at(byte_buffer *bb, uint32_t index) {
	return *(uint32_t*)(bb->buf+index);
}

uint64_t bb_get_long(byte_buffer *bb) {
	uint64_t ret = *(uint64_t*)(bb->buf+bb->pos);
	bb->pos += sizeof(uint64_t);
	return ret;
}

uint64_t bb_get_long_at(byte_buffer *bb, uint32_t index) {
	return *(uint64_t*)(bb->buf+index);
}

uint16_t bb_get_short(byte_buffer *bb) {
	uint16_t ret = *(uint16_t*)(bb->buf+bb->pos);
	bb->pos += sizeof(uint16_t);
	return ret;
}

uint16_t bb_get_short_at(byte_buffer *bb, uint32_t index) {
	return *(uint16_t*)(bb->buf+index);
}

// Relative write of the entire contents of another ByteBuffer (src)
void bb_put_bb(byte_buffer *dest, byte_buffer* src) {
	uint32_t i = src->pos;

	while(i < src->len) {
		bb_put(dest, src->buf[i]);
		i++;
	}
}

void bb_put(byte_buffer *bb, uint8_t value) {
	if(bb->pos >= bb->len)
		return;

	bb->buf[bb->pos++] = value;
}

void bb_put_at(byte_buffer *bb, uint8_t value, uint32_t index) {
	if(index >= bb->len)
		return;

	bb->buf[index] = value;
}

void bb_put_bytes(byte_buffer *bb, uint8_t *arr, size_t len) {
	for(uint32_t i = 0; i < len; i++) {
		bb_put(bb, arr[i]);
		i++;
	}
}

void bb_put_bytes_at(byte_buffer *bb, uint8_t *arr, size_t len, uint32_t index) {
	for(uint32_t i = index; i < bb->len; i++) {
		bb_put_at(bb, arr[i], index+i);
		i++;
	}
}

void bb_put_double(byte_buffer *bb, double value) {
	*(double*)(bb->buf+bb->pos) = value;
	bb->pos += sizeof(double);
}

void bb_put_double_at(byte_buffer *bb, double value, uint32_t index) {
	*(double*)(bb->buf+index) = value;
}

void bb_put_float(byte_buffer *bb, float value) {
	*(float*)(bb->buf+bb->pos) = value;
	bb->pos += sizeof(float);
}

void bb_put_float_at(byte_buffer *bb, float value, uint32_t index) {
	*(float*)(bb->buf+index) = value;
}

void bb_put_int(byte_buffer *bb, uint32_t value) {
	*(uint32_t*)(bb->buf+bb->pos) = value;
	bb->pos += sizeof(uint32_t);
}

void bb_put_int_at(byte_buffer *bb, uint32_t value, uint32_t index) {
	*(uint32_t*)(bb->buf+index) = value;
}

void bb_put_long(byte_buffer *bb, uint64_t value) {
	*(uint64_t*)(bb->buf+bb->pos) = value;
	bb->pos += sizeof(uint64_t);
}

void bb_put_long_at(byte_buffer *bb, uint64_t value, uint32_t index) {
	*(uint64_t*)(bb->buf+index) = value;
}

void bb_put_short(byte_buffer *bb, uint16_t value) {
	*(uint16_t*)(bb->buf+bb->pos) = value;
	bb->pos += sizeof(uint16_t);
}

void bb_put_short_at(byte_buffer *bb, uint16_t value, uint32_t index) {
	*(uint16_t*)(bb->buf+index) = value;
}

