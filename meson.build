project(
    'libhal',
    ['cpp'],
    version: '0.1.0',
    default_options: [
        'cpp_std=c++23',
        'default_library=static',
        'b_lto=true',
        'cpp_eh=none',
        'cpp_rtti=false',
        'optimization=s',
        'warning_level=3',
        'debug=false',
        'strip=true',
    ],
    meson_version: '>=1.10',
)

size = find_program('size')
ld = find_program('ld')
cc = find_program('cx')
cpp = find_program('cpp')

# configs from properties

ld_conf = configuration_data()
ld_conf.set('flash_origin', meson.get_property('flash_origin'))
ld_conf.set('flash_kb', meson.get_property('flash_kb'))
ld_conf.set('ram_origin', meson.get_property('ram_origin'))
ld_conf.set('ram_kb', meson.get_property('ram_kb'))
ld_conf.set('heap_size', meson.get_property('heap_size'))
ld_conf.set('stack_size', meson.get_property('stack_size'))

ld_script_in = meson.project_source_root() / 'linker/linker.ld.in'

ld_script = configure_file(
  input: ld_script_in,
  output: 'linker.ld',
  configuration: ld_conf,
  )

clk_conf = confguration_data()
clk_conf.set('hse_hz', meson.get_external_property('hse_hz', 0))
clk_conf.set('hsi_hz', meson.get_external_property('hsi_hz', 0))
clk_conf.set('lse_hz', meson.get_external_property('lse_hz', 0))
clk_conf.set('lsi_hz', meson.get_external_property('lsi_hz', 0))
clk_conf.set('sysclk_hz', meson.get_external_property('sysclk_hz'))
clk_conf.set('hclk_hz', meson.get_external_property('hclk_hz'))
clk_conf.set('pclk1_hz', meson.get_external_property('pclk1_hz'))
clk_conf.set('pclk2_hz', meson.get_external_property('pclk2_hz'))
clk_conf.set('apb1_timer_hz', meson.get_external_property('apb1_timer_hz', 0))
clk_conf.set('apb2_timer_hz', meson.get_external_property('apb2_timer_hz', 0))

clk_config_in = meson.project_source_root() / 'include/clk_config.hpp.in' 
clk_config = configure_file(
  input: clk_config_in,
  output: 'clk_config.hpp',
  configuration: clk_conf,
  )


# dependencies
cib_sub = subproject('compile-time-init-build')
cib_dep = cib_sub.get_variable('cib_dep')

stm32_sub = subproject('stm32', default_options: ['mcu=@0@'.format(meson.get_external_property('stm32_mcu'))])
stm32_dep = stm32_sub.get_variable('stm32_dep')

hal_dep = declare_dependency(
  include_directories: hal_inc,
  dependencies: [stm32_dep, cib_dep],
  link_args: ['-T' + ld_script.full_path()],
  )

if not meson.is_subproject()
    ut_sub = subproject('ut')
    ut_dep = ut_sub.get_variable('boostut_dep')

    subdir('docs')
    subdir('tests')
endif
