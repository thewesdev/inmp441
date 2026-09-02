#include "inmp441.h"

#include <driver/i2s_std.h>

static i2s_chan_handle_t rx_handle;

static i2s_chan_config_t chan_conf = I2S_CHANNEL_DEFAULT_CONFIG(CONFIG_INMP441_I2S_CHANNEL, I2S_ROLE_MASTER);
static i2s_std_config_t std_conf = {
	.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
	.slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
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
		}
	}
};

void inmp441_init() {
	i2s_new_channel(&chan_conf, NULL, &rx_handle);
	i2s_channel_init_std_mode(rx_handle, &std_conf);
	i2s_channel_enable(rx_handle);
}

void inmp441_stop() {
	i2s_channel_disable(rx_handle);
	i2s_del_channel(rx_handle);
}
