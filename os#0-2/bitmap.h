#ifndef __LIB_KERNEL_BITMAP_H
#define __LIB_KERNEL_BITMAP_H

#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>

/* Bitmap abstract data type. */

/* Creation and destruction. */
struct bitmap *bitmap_create (size_t bit_cnt);
struct bitmap *bitmap_create_in_buf (size_t bit_cnt, void *, size_t byte_cnt);
size_t bitmap_buf_size (size_t bit_cnt);
void bitmap_destroy (struct bitmap *);

/* Bitmap size. */
size_t bitmap_size (const struct bitmap *);

/* Setting and testing single bits. */
void bitmap_set (struct bitmap *, size_t idx, bool);
void bitmap_mark (struct bitmap *, size_t idx);
void bitmap_reset (struct bitmap *, size_t idx);
void bitmap_flip (struct bitmap *, size_t idx);
bool bitmap_test (const struct bitmap *, size_t idx);

/* Setting and testing multiple bits. */
void bitmap_set_all (struct bitmap *, bool);
void bitmap_set_multiple (struct bitmap *, size_t start, size_t cnt, bool);
size_t bitmap_count (const struct bitmap *, size_t start, size_t cnt, bool);
bool bitmap_contains (const struct bitmap *, size_t start, size_t cnt, bool);
bool bitmap_any (const struct bitmap *, size_t start, size_t cnt);
bool bitmap_none (const struct bitmap *, size_t start, size_t cnt);
bool bitmap_all (const struct bitmap *, size_t start, size_t cnt);

/* Finding set or unset bits. */
#define BITMAP_ERROR SIZE_MAX
size_t bitmap_scan (const struct bitmap *, size_t start, size_t cnt, bool);
size_t bitmap_scan_and_flip (struct bitmap *, size_t start, size_t cnt, bool);

/* File input and output. */
#ifdef FILESYS
struct file;
size_t bitmap_file_size (const struct bitmap *);
bool bitmap_read (struct bitmap *, struct file *);
bool bitmap_write (const struct bitmap *, struct file *);
#endif

/* Debugging. */
void bitmap_dump (const struct bitmap *);

//proj #0-2
struct bitmap *bitmap_expand(struct bitmap *bitmap, int size);

/* Expand */
void bitmap_new(struct bitmap **bitmap, size_t bit_cnt);
void bitmap_delete(struct bitmap **bitmap);
void bitmap_print(struct bitmap *bitmap);

//For Shell
void ibitmap_dump(struct bitmap** bitmap);
void ibitmap_size(struct bitmap** bitmap);


void ibitmap_set_all(struct bitmap** bitmap, char* boolean);

void ibitmap_mark(struct bitmap** bitmap, int num);
void ibitmap_expand(struct bitmap** bitmap, int num);
void ibitmap_flip(struct bitmap** bitmap, int num);
void ibitmap_reset(struct bitmap** bitmap, int num);
void ibitmap_test(struct bitmap** bitmap, int num);

void ibitmap_all(struct bitmap** bitmap, int num1, int num2);
void ibitmap_any(struct bitmap** bitmap, int num1, int num2);
void ibitmap_none(struct bitmap** bitmap, int num1, int num2);

void ibitmap_set(struct bitmap** bitmap, int num1, char* boolean);

void ibitmap_contains(struct bitmap** bitmap, int num1, int num2, char* boolean);
void ibitmap_count(struct bitmap** bitmap, int num1, int num2, char* boolean);
void ibitmap_scan(struct bitmap** bitmap, int num1, int num2, char* boolean);
void ibitmap_scan_and_flip(struct bitmap** bitmap, int num1, int num2, char* boolean);
void ibitmap_set_multiple(struct bitmap** bitmap, int num1, int num2, char* boolean);
#endif /* lib/kernel/bitmap.h */
