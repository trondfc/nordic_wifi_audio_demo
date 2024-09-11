#include "audio_usb.h"

#include <data_fifo.h>
DATA_FIFO_DEFINE(fifo_usb_tx, 100, 512);
DATA_FIFO_DEFINE(fifo_usb_rx, 100, 512);

#define BLOCK_SIZE_BYTES (FRAME_SIZE_BYTES / CONFIG_FIFO_FRAME_SPLIT_NUM)
#define FRAME_SIZE_BYTES ((CONFIG_I2S_LRCK_FREQ_HZ / 1000 * 10) * CONFIG_I2S_CH_NUM * CONFIG_AUDIO_BIT_DEPTH_OCTETS)

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_MAIN_LOG_LEVEL);

void streamctrl_send(void const *const data, size_t size, uint8_t num_ch)
{
	LOG_INF("Sending audio controlls");
}

int main(void)
{
  LOG_INF("Hello, world!");

  int ret;

  if (!fifo_usb_tx.initialized) {
		ret = data_fifo_init(&fifo_usb_tx);
		if (ret != 0) {
      LOG_ERR("Failed to set up tx FIFO: %d", ret);
      return ret;
    }
	}

  if (!fifo_usb_rx.initialized) {
		ret = data_fifo_init(&fifo_usb_rx);
		if (ret != 0) {
      LOG_ERR("Failed to set up rx FIFO: %d", ret);
      return ret;
    }
	}

  ret = audio_usb_init();
  if (ret != 0) {
    LOG_ERR("Failed to initialize audio USB: %d", ret);
    return ret;
  }

  ret = audio_usb_start(&fifo_usb_tx, &fifo_usb_rx);
  if (ret != 0) {
    LOG_ERR("Failed to start audio USB: %d", ret);
    return ret;
  }

  void *tmp_pcm_raw_data[CONFIG_FIFO_FRAME_SPLIT_NUM];
  static size_t pcm_block_size;
  char pcm_raw_data[FRAME_SIZE_BYTES];
  uint32_t blocks_alloced_num;
  uint32_t blocks_locked_num;

 
  while(1) {
    if (get_rx_first_data()) {
      ret = data_fifo_num_used_get(&fifo_usb_rx, &blocks_alloced_num, &blocks_locked_num);
      if (ret != 0) {
        LOG_ERR("Failed to get num used in FIFO: %d", ret);
        return ret;
      }
      LOG_INF("RX alloced: %d, locked: %d", blocks_alloced_num, blocks_locked_num);

        ret = data_fifo_pointer_last_filled_get(&fifo_usb_rx, &tmp_pcm_raw_data,
                  &pcm_block_size, K_FOREVER);
        if (ret != 0) {
          LOG_ERR("Failed to get last filled block: %d", ret);
          return ret;
        }
        // memcpy(pcm_raw_data, tmp_pcm_raw_data, pcm_block_size);
        data_fifo_block_free(&fifo_usb_rx, tmp_pcm_raw_data);

        // LOG_INF("Got audio data, size: %d", pcm_block_size);
        // LOG_INF("Data: %s", pcm_raw_data);
      
    }
    k_sleep(K_MSEC(10));
    
  }

  return 0;
}