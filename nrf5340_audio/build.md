## How to build and flash the firmware

### unicast server (CIS Headset Mode):
build
```
west build -b nrf5340_audio_dk/nrf5340/cpuapp --sysbuild --pristine -d build_unicast_server_rx  -- -DCONFIG_AUDIO_DEV=1
```
flash
```
 west flash --erase --build-dir build_unicast_server_rx/
```

### unicast client (CIS Gateway Mode):
build
```
west build -b nrf5340_audio_dk/nrf5340/cpuapp --sysbuild --pristine -d build_unicast_client_tx  -- -DCONFIG_AUDIO_DEV=2
```
flash
```
 west flash --erase --build-dir build_unicast_client_tx/
```
