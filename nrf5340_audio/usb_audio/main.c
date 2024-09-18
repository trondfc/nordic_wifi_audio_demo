#include "audio_usb.h"
#include "audio_system.h"
#include "nrf5340_audio_dk.h"
#include "audio_datapath.h"

#include <data_fifo.h>
// DATA_FIFO_DEFINE(fifo_usb_tx, 100, 512);
// DATA_FIFO_DEFINE(fifo_usb_rx, 100, 512);

// #define BLOCK_SIZE_BYTES (FRAME_SIZE_BYTES / CONFIG_FIFO_FRAME_SPLIT_NUM)
// #define FRAME_SIZE_BYTES ((CONFIG_I2S_LRCK_FREQ_HZ / 1000 * 10) * CONFIG_I2S_CH_NUM * CONFIG_AUDIO_BIT_DEPTH_OCTETS)

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_MAIN_LOG_LEVEL);

#include "streamctrl.h"
static enum stream_state strm_state = STATE_PAUSED;

uint8_t stream_state_get(void)
{
	return strm_state;
}

void streamctrl_send(void const *const data, size_t size, uint8_t num_ch)
{
	LOG_INF("Sending audio controlls");
}

int main(void)
{
  LOG_INF("Hello, world!");

  int ret;

  ret = nrf5340_audio_dk_init();
	if (ret != 0) {
		LOG_ERR("Failed to initialize audio DK: %d", ret);
		return ret;
	}
	
	ret = audio_system_init();
	if (ret != 0) {
		LOG_ERR("Failed to initialize audio system: %d", ret);
		return ret;
	}

  // ret = audio_system_config_set(48000, 96000, VALUE_NOT_SET);
	// if(ret != 0) {
  //   LOG_ERR("Failed to set sample- and bitrate: %d", ret);
  //   return ret;
  // }

  audio_system_start();
  audio_system_encoder_start();

  	/* Play a test tone */
	ret = audio_datapath_tone_play(500,5000,1);
	if (ret != 0) {
		LOG_ERR("Failed to play tone: %d", ret);
		return ret;
	}

  ret = audio_usb_init();
  if (ret != 0) {
    LOG_ERR("Failed to initialize audio USB: %d", ret);
    return ret;
  }

  struct data_fifo* fifo_tx = audio_system_fifo_tx_get();
  // struct data_fifo* fifo_rx = audio_system_fifo_rx_get();
  struct data_fifo* fifo_rx = audio_datapath_fifo_tx_get();

  if (fifo_tx == NULL || fifo_rx == NULL) {
    LOG_ERR("Failed to get FIFOs");
    return -ENODEV;
  }


  if(!fifo_tx->initialized){
    LOG_WRN("TX FIFO not initialized, initializing");
    ret = data_fifo_init(fifo_tx);
    if (ret != 0) {
      LOG_ERR("Failed to initialize FIFO: %d", ret);
      return ret;
    }
  }

  if(!fifo_rx->initialized){
    LOG_WRN("RX FIFO not initialized, initializing");
    ret = data_fifo_init(fifo_rx);
    if (ret != 0) {
      LOG_ERR("Failed to initialize FIFO: %d", ret);
      return ret;
    }
  }

  LOG_INF("Got FIFOs");
  LOG_INF("TX: %p, RX: %p", fifo_tx, fifo_rx);

   ret = audio_usb_start(fifo_tx, fifo_rx);
  if (ret != 0) {
    LOG_ERR("Failed to start audio USB: %d", ret);
    return ret;
  }


  return 0;
}