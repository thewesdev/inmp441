#include <freertos/FreeRTOS.h>
#include <inmp441.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <driver/uart.h>

static int32_t *buffer = NULL;

void app_main(void) {
	uint16_t buffer_size = 512;
	uint16_t sample_rate = 16000;

	buffer = (int32_t *)malloc(buffer_size * sizeof(int32_t));
	int16_t *sample_buffer = (int16_t *)malloc(buffer_size * sizeof(int16_t));

	inmp441_init(sample_rate, buffer_size);

	ESP_ERROR_CHECK(uart_driver_install(
		UART_NUM_0, 256, buffer_size * sizeof(int16_t), 0, NULL, 0));
	ESP_ERROR_CHECK(uart_set_baudrate(UART_NUM_0, 921600));

	while (1) {
		inmp441_read_samples(buffer);

		for (size_t i = 0; i < buffer_size; i++)
			sample_buffer[i] = buffer[i] >> 16;

		uart_write_bytes(UART_NUM_0, (const char *)sample_buffer,
						 buffer_size * sizeof(int16_t));
	}
}
