zephyr_include_directories(include)
zephyr_include_directories(${APPLICATION_SOURCE_DIR}/include)
zephyr_library()

if(CONFIG_SHIELD_PROSPECTOR_ADAPTER)

        add_subdirectory(${ZEPHYR_CURRENT_MODULE_DIR}/drivers/display)
        add_subdirectory(${ZEPHYR_CURRENT_MODULE_DIR}/modules/lvgl)

        zephyr_library_sources(src/events/split_central_status_changed.c)
        set_source_files_properties(
                ${APPLICATION_SOURCE_DIR}/src/split/bluetooth/central.c
                TARGET_DIRECTORY app
                PROPERTIES HEADER_FILE_ONLY ON)
        zephyr_library_sources(src/split/bluetooth/central.c)

endif()