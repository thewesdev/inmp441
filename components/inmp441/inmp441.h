#ifndef COMPONENT_INMP441_H
#define COMPONENT_INMP441_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "esp_err.h"

esp_err_t inmp441_init(void);
esp_err_t inmp441_stop(void);
esp_err_t inmp441_read_samples(int32_t *buffer);

#ifdef __cplusplus
}
#endif

#endif // COMPONENT_INMP441_H
