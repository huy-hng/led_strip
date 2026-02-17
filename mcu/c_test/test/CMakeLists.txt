if (TARGET tinyusb_device)
	add_executable(pico
		test.c
	)

	# pull in common dependencies
	target_link_libraries(pico pico_stdlib)

	# enable usb output, disable uart output
	pico_enable_stdio_usb(pico 1)
	pico_enable_stdio_uart(pico 0)

	# create map/bin/hex/uf2 file etc.
	pico_add_extra_outputs(pico)

	# add url via pico_set_program_url
	example_auto_set_url(pico)
elseif(PICO_ON_DEVICE)
	message("Skipping hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()
