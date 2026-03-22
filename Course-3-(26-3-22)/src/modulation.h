#ifndef MODULATION_H
#define MODULATION_H

// 注意：题目中的 unsigned double 在 C/C++ 中是非法类型，这里统一改为 double
int generate_cover_signal(double *cover, const int size);
int simulate_digital_modulation_signal(unsigned char *message, const int size);
int simulate_analog_modulation_signal(double *message, const int size);

int modulate_digital_frequency(double *cover, const int cover_len, const unsigned char *message,
                               const int msg_len);
int modulate_analog_frequency(double *cover, const int cover_len, const double *message, const int msg_len);

int modulate_digital_amplitude(double *cover, const int cover_len, const unsigned char *message,
                               const int msg_len);
int modulate_analog_amplitude(double *cover, const int cover_len, const double *message, const int msg_len);

int modulate_digital_phase(double *cover, const int cover_len, const unsigned char *message, const int msg_len);
int modulate_analog_phase(double *cover, const int cover_len, const double *message, const int msg_len);

#endif
