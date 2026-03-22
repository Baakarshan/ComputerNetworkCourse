#ifndef MULTIPLEX_H
#define MULTIPLEX_H

#include <cstddef>

// 题目给定接口：默认采用“同步时分复用”
int multiplex(unsigned char *c, const int c_size, const unsigned char *a, const int a_len,
              const unsigned char *b, const int b_len);
int demultiplex(unsigned char *a, const int a_size, unsigned char *b, const int b_size,
                const unsigned char *c, const int c_len);

// 为了演示四种方式，额外提供独立接口
int multiplex_sync_tdm(unsigned char *c, int c_size, const unsigned char *a, int a_len,
                       const unsigned char *b, int b_len);
int demultiplex_sync_tdm(unsigned char *a, int a_size, unsigned char *b, int b_size,
                         const unsigned char *c, int c_len);

int multiplex_stat_tdm(unsigned char *c, int c_size, const unsigned char *a, int a_len,
                       const unsigned char *b, int b_len);
int demultiplex_stat_tdm(unsigned char *a, int a_size, unsigned char *b, int b_size,
                         const unsigned char *c, int c_len);

int multiplex_fdm(unsigned char *c, int c_size, const unsigned char *a, int a_len,
                  const unsigned char *b, int b_len);
int demultiplex_fdm(unsigned char *a, int a_size, unsigned char *b, int b_size,
                    const unsigned char *c, int c_len);

int multiplex_cdm(unsigned char *c, int c_size, const unsigned char *a, int a_len,
                  const unsigned char *b, int b_len);
int demultiplex_cdm(unsigned char *a, int a_size, unsigned char *b, int b_size,
                    const unsigned char *c, int c_len);

#endif
