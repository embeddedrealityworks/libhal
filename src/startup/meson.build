mcu_core = meson.get_external_property('mcu_core')

if mcu_core in ['cortex-m0', 'cortex-m0+']
    startup_src = files('startup-armv6.cpp')
elif mcu_core in ['cortex-m3', 'cortex-m4', 'cortex-m7']
    startup_src = files('startup-armv7.cpp')
elif mcu_core in ['cortex-m33', 'cortex-m55']
    startup_src = files('startup-armv8.cpp')
else
    error('Unknown mcu_core: ' + mcu_core)
endif

message('startup: ' + mcu_core)
