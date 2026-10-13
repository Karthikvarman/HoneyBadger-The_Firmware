#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include <stddef.h>

uint16_t CRC16_CCITT(const uint8_t *data, size_t len);

uint8_t CRC8_Calculate(const uint8_t *data, size_t len);

#endif
