# Bluetooth profile indicator for ZMK.
if (CONFIG_ZMK_RGB_BLE_INDICATOR)
  zephyr_library()
  zephyr_library_sources(src/rgb_ble_indicator.c)
  zephyr_include_directories(${APPLICATION_SOURCE_DIR}/include)
endif()
