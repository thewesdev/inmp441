# INMP441

## Features

- I2S driver setup (standard mode, mono, 32-bit) for the INMP441 MEMS microphone
- init, stop and blocking read functions
- background sensor task that pushes samples into a queue, decoupling I2S reads from consumer code
- I2S channel and pin configuration via menuconfig

## I2S

sample rate: 16 kHz (fixed)
slot mode: mono, left channel
data/slot bit width: 32 bits

> Sample rate and block size are currently hardcoded constants. Dynamic configuration (runtime-settable sample rate, block size, etc.) is planned for a future version.

## Menuconfig

```bash
idf.py menuconfig
```

### Path

Component config -> INMP441

### Default Configs

| INMP441     | Type     | Macro                      | Values |
| ----------- | -------- | -------------------------- | ------ |
| I2S Channel | int      | CONFIG_INMP441_I2S_CHANNEL | 0      |
| SCK         | GPIO_NUM | CONFIG_INMP441_SCK_PIN     | 47     |
| WS          | GPIO_NUM | CONFIG_INMP441_WS_PIN      | 45     |
| SD          | GPIO_NUM | CONFIG_INMP441_SD_PIN      | 48     |
| L/R         | GPIO_NUM | CONFIG_INMP441_LR_PIN      | 46     |

## Dependencies

- ESP-IDF

## Installation

### IDF Component Registry

```bash
idf.py add-dependency thewesdev/inmp441
```

For update newest versions

```bash
idf.py update-dependencies
```

### Github

```bash
git clone https://github.com/thewesdev/inmp441
```

## Code Examples

```c
#include <stdio.h>

#include <esp_log.h>
#include <freertos/FreeRTOS.h>

#include "inmp441.h"

void app_main(void) {
	inmp441_init();

	int32_t buffer[512];

	while (1) {
		esp_err_t err = inmp441_read_samples(buffer);

		if (err != ESP_OK) {
			ESP_LOGE("APP", "failed to read samples: %s", esp_err_to_name(err));
			continue;
		}

		printf("first sample: %" PRId32 "\n", buffer[0]);
	}
}
```
