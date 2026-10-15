if(CONFIG_ZMK_APP_LAYER_AUTOSWITCH)
  zephyr_library()
  zephyr_library_sources(src/app_layer_autoswitch.c)
endif()
