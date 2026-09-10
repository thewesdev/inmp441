#include "inmp441.h"

#include <driver/i2s_std.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#define SAMPLE_RATE 16000
#define BUFFER_SIZE 512
#define QUEUE_LENGTH 4

static const char *TAG = "INMP441";

static i2s_chan_handle_t rx_handle = NULL;
static QueueHandle_t sample_queue = NULL;
static TaskHandle_t sensor_task_handle = NULL;

static int32_t buffer[BUFFER_SIZE];

static void inmp441_sensor(void *args) {
	(void)args;

	while (1) {
		size_t bytes_read;

		esp_err_t err = i2s_channel_read(rx_handle, buffer, sizeof(buffer),
										 &bytes_read, 50);

		if (err != ESP_OK) {
			ESP_LOGE(TAG, "failed to read data: %s", esp_err_to_name(err));
			continue;
		}

		if (bytes_read != sizeof(buffer)) {
			ESP_LOGW(TAG, "expected " PRId32 ", got " PRId32, sizeof(buffer),
					 bytes_read);
			continue;
		}

		if (xQueueSend(sample_queue, buffer, portMAX_DELAY) != pdTRUE)
			ESP_LOGW(TAG, "failed to send samples to queue");
	}
}

esp_err_t inmp441_init(void) {
	esp_err_t err;

	sample_queue = xQueueCreate(QUEUE_LENGTH, BUFFER_SIZE * sizeof(int32_t));

	if (sample_queue == NULL) {
		ESP_LOGE(TAG, "Failed to create sample queue");
		return ESP_ERR_NO_MEM;
	}

	i2s_chan_config_t chan_cfg =
		I2S_CHANNEL_DEFAULT_CONFIG(CONFIG_INMP441_I2S_CHANNEL, I2S_ROLE_MASTER);

	// clang-format off
	i2s_std_config_t std_cfg = {
		.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
		.slot_cfg = {
			.data_bit_width = I2S_DATA_BIT_WIDTH_32BIT,
			.slot_bit_width = I2S_DATA_BIT_WIDTH_32BIT,
			.ws_width = I2S_DATA_BIT_WIDTH_32BIT,
			.slot_mode = I2S_SLOT_MODE_MONO,
			.slot_mask = I2S_STD_SLOT_LEFT,
			.ws_pol = false,
			.bit_shift = true,
			.left_align = true,
			.big_endian = false,
			.bit_order_lsb = false,
		},
		.gpio_cfg = {
			.bclk = CONFIG_INMP441_SCK_PIN,
			.ws = CONFIG_INMP441_WS_PIN,
			.din = CONFIG_INMP441_SD_PIN,
			.dout = I2S_GPIO_UNUSED,
			.mclk = I2S_GPIO_UNUSED,
			.invert_flags = {
				.bclk_inv = false,
				.mclk_inv = false,
				.ws_inv = false,
			},
		},
	};
	// clang-format on

	err = i2s_new_channel(&chan_cfg, NULL, &rx_handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "failed to create new i2s channel: %s",
				 esp_err_to_name(err));

		return err;
	}

	err = i2s_channel_init_std_mode(rx_handle, &std_cfg);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "failed to init i2s channel standard mode: %s",
				 esp_err_to_name(err));

		i2s_del_channel(rx_handle);
		rx_handle = NULL;

		return err;
	}

	err = i2s_channel_enable(rx_handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "failed to enable i2s channel: %s", esp_err_to_name(err));

		i2s_del_channel(rx_handle);
		rx_handle = NULL;

		return err;
	}

	BaseType_t task_err = xTaskCreate(inmp441_sensor, "INMP441 Sensor", 2048,
									  NULL, 1, &sensor_task_handle);

	if (task_err != pdPASS) {
		ESP_LOGE(TAG, "failed to create sensor task");

		i2s_channel_disable(rx_handle);
		i2s_del_channel(rx_handle);

		rx_handle = NULL;

		return ESP_ERR_NO_MEM;
	}

	ESP_LOGI(TAG, "INMP441 initialized");

	return ESP_OK;
}

esp_err_t inmp441_stop(void) {
	esp_err_t err;

	if (sensor_task_handle != NULL) {
		vTaskDelete(sensor_task_handle);
		sensor_task_handle = NULL;
	}

	if (rx_handle != NULL) {
		err = i2s_channel_disable(rx_handle);
		if (err != ESP_OK)
			return err;

		err = i2s_del_channel(rx_handle);
		if (err != ESP_OK)
			return err;

		rx_handle = NULL;
	}

	return ESP_OK;
}

esp_err_t inmp441_read_samples(int32_t *buffer) {
	if (buffer == NULL)
		return ESP_ERR_INVALID_ARG;

	if (xQueueReceive(sample_queue, buffer, portMAX_DELAY) != pdTRUE)
		return ESP_ERR_TIMEOUT;

	return ESP_OK;
}
