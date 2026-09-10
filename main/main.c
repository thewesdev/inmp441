#include <freertos/FreeRTOS.h>
#include <inmp441.h>
#include <inttypes.h>
#include <stdio.h>

#include <driver/uart.h>

static int32_t buffer[512];

void app_main(void) {
	int16_t sample_buffer[512];
	inmp441_init();

	ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 256, 2048, 0, NULL, 0));
	ESP_ERROR_CHECK(uart_set_baudrate(UART_NUM_0, 921600));

	while (1) {
		inmp441_read_samples(buffer);

		for (size_t i = 0; i < 512; i++)
			sample_buffer[i] = buffer[i] >> 16;

		uart_write_bytes(UART_NUM_0, (const char *)sample_buffer,
						 sizeof(sample_buffer));
	}
}
