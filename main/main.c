#include <freertos/FreeRTOS.h>
#include <inmp441.h>
#include <inttypes.h>
#include <stdio.h>

#include <driver/uart.h>

static const uint8_t SYNC_MARKER[4] = {0xAA, 0x55, 0xAA, 0x55};

void app_main(void) {
	int32_t buffer[512];
	inmp441_init();

	ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 256, 2048, 0, NULL, 0));
	ESP_ERROR_CHECK(uart_set_baudrate(UART_NUM_0, 921600));

	while (1) {
		inmp441_read_samples(buffer);

		for (size_t i = 0; i < 512; i++)
			buffer[i] = buffer[i] >> 8;

		uart_write_bytes(UART_NUM_0, (const char *)SYNC_MARKER,
						 sizeof(SYNC_MARKER));
		uart_write_bytes(UART_NUM_0, (const char *)buffer, sizeof(buffer));
	}
}
