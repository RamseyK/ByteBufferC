/**
   ByteBuffer (C implementation)
   bytebuffer.h
   Copyright 2011-2025 Ramsey Kant

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

#ifndef _BYTEBUFFER_H_
#define _BYTEBUFFER_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

// Default number of bytes to allocate in the backing buffer if no size is provided
#define BB_DEFAULT_SIZE 4096

typedef struct byte_buffer_t {
    uint32_t pos; // Read/Write position
    bool wrapped; // True if this byte buffer is a wrapping buf
    size_t len; // Length of buf array
    uint8_t *buf;
} byte_buffer;

// Memory allocation functions
byte_buffer *bb_new_wrap(uint8_t *buf, size_t len);
byte_buffer *bb_new_copy(const uint8_t *buf, size_t len);
byte_buffer *bb_new(size_t len);
byte_buffer *bb_new_from_file(const char *path, const char *fopen_opts);
byte_buffer *bb_new_default();
bool bb_resize(byte_buffer* bb, size_t new_len);
void bb_free(byte_buffer *bb);

// Utility
void bb_skip(byte_buffer *bb, size_t len);
size_t bb_bytes_left(const byte_buffer *bb);
void bb_clear(const byte_buffer *bb);
byte_buffer *bb_clone(const byte_buffer *bb);
bool bb_equals(const byte_buffer* bb1, const byte_buffer* bb2);
void bb_replace(byte_buffer *bb, uint8_t key, uint8_t rep, uint32_t start, bool firstOccuranceOnly);
void bb_print_ascii(const byte_buffer *bb);
void bb_print_hex(const byte_buffer *bb);

// Read functions
uint8_t bb_peek(const byte_buffer *bb);
uint8_t bb_get(byte_buffer *bb);
uint8_t bb_get_at(const byte_buffer *bb, uint32_t index);
void bb_get_bytes_in(byte_buffer *bb, uint8_t *dest, size_t len);
void bb_get_bytes_at_in(byte_buffer *bb, uint32_t index, uint8_t *dest, size_t len);
uint8_t *bb_get_bytes(byte_buffer *bb, size_t len);
// uint8_t *bb_get_bytes_at(byte_buffer *bb, size_t len, uint32_t index);
double bb_get_double(byte_buffer *bb);
double bb_get_double_at(byte_buffer *bb, uint32_t index);
float bb_get_float(byte_buffer *bb);
float bb_get_float_at(byte_buffer *bb, uint32_t index);
uint32_t bb_get_int(byte_buffer *bb);
uint32_t bb_get_int_at(byte_buffer *bb, uint32_t index);
uint64_t bb_get_long(byte_buffer *bb);
uint64_t bb_get_long_at(byte_buffer *bb, uint32_t index);
uint16_t bb_get_short(byte_buffer *bb);
uint16_t bb_get_short_at(byte_buffer *bb, uint32_t index);

// Put functions (simply drop bytes until there is no more room)
void bb_put_bb(byte_buffer *dest, const byte_buffer* src);
void bb_put(byte_buffer *bb, uint8_t value);
void bb_put_at(byte_buffer *bb, uint8_t value, uint32_t index);
void bb_put_bytes(byte_buffer *bb, const uint8_t *arr, size_t len);
void bb_put_bytes_at(byte_buffer *bb, const uint8_t *arr, size_t len, uint32_t index);
void bb_put_double(byte_buffer *bb, double value);
void bb_put_double_at(byte_buffer *bb, double value, uint32_t index);
void bb_put_float(byte_buffer *bb, float value);
void bb_put_float_at(byte_buffer *bb, float value, uint32_t index);
void bb_put_int(byte_buffer *bb, uint32_t value);
void bb_put_int_at(byte_buffer *bb, uint32_t value, uint32_t index);
void bb_put_long(byte_buffer *bb, uint64_t value);
void bb_put_long_at(byte_buffer *bb, uint64_t value, uint32_t index);
void bb_put_short(byte_buffer *bb, uint16_t value);
void bb_put_short_at(byte_buffer *bb, uint16_t value, uint32_t index);

// TODO: Insert functions (make room (potentially resizing), then drop at the specified location)

#endif
