
#include "audio_system.h"
#include "nrf5340_audio_dk.h"
#include "streamctrl.h"
#include "audio_datapath.h"
#include "audio_usb.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_MAIN_LOG_LEVEL);

static enum stream_state strm_state = STATE_PAUSED;

void streamctrl_send(void const *const data, size_t size, uint8_t num_ch)
{
	LOG_INF("Sending audio controlls");
}

static void stream_state_set(enum stream_state stream_state_new)
{
	strm_state = stream_state_new;
}

uint8_t stream_state_get(void)
{
	return strm_state;
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

  ret = audio_usb_init();
  if (ret != 0) {
    LOG_ERR("Failed to initialize audio USB: %d", ret);
    return ret;
  }

  // ret = audio_usb_start(&usb_fifo_tx, &usb_fifo_rx);
  // if (ret != 0) {
  //   LOG_ERR("Failed to start audio USB: %d", ret);
  //   return ret;
  // }

  audio_system_start();

  
  ret = audio_datapath_tone_play(500,10000,1);
  if (ret != 0) {
    LOG_ERR("Failed to play tone: %d", ret);
    return ret;
  }

  return 0;
}