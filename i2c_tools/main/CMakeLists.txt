idf_component_register(SRCS "test.c"
                            "func_i2ctools.c"
                       PRIV_REQUIRES fatfs esp_driver_i2c
                       INCLUDE_DIRS ".")
