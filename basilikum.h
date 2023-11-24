#ifndef ROKTOOLS_BASILIKUM_H
#define ROKTOOLS_BASILIKUM_H

#ifdef __cplusplus
extern "C" {
#endif

void basilikum_init();

char* basilikum_encode(const char* input, const unsigned long long secret);

char* basilikum_decode(const char* input, const unsigned long long secret);

#ifdef __cplusplus
}
#endif

#endif //  ROKTOOLS_BASILIKUM_H
