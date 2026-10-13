#include "crc.h"

uint16_t CRC16_CCITT(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFFU;

    if (data == (const uint8_t*)0) {
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        crc ^= ((uint16_t)data[i]) << 8;
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x8000U) {
                crc = (crc << 1) ^ 0x1021U;
            } else {
                crc = crc << 1;
            }
        }
    }

    return crc;
}

uint8_t CRC8_Calculate(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00U;

    if (data == (const uint8_t*)0) {
        return 0;
    }

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x80U) {
                crc = (crc << 1) ^ 0x07U;
            } else {
                crc = crc << 1;
            }
        }
    }

    return crc;
}
