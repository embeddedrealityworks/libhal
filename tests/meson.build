native_tests = ['units']
target_tests = ['units']

# ── Host tests (fast, no hardware required) ──────────────────────────────────

foreach tl : native_tests
  test_name = 'test_' + tl
  test_exe = executable(
    test_name,
    tl + '.cpp',
    dependencies : [ut_dep],
    include_directories: hal_inc,
    native: true
    )
  test(test_name, test_exe)
endforeach

# ── Renode tests (cross-compiled, simulated on target) ────────────────────────
#
# Each test is compiled for the ARM target, linked with the baremetal startup
# and semihosting stubs, then run under Renode via Robot Framework.
#
# Prerequisites:
#   - renode-test  (ships with Renode, wraps Robot Framework)
#   - Renode >= 1.14  (cpu EnableSemihosting, SYS_EXIT support)

renode_test = find_program(
  'renode-test',
  '/Applications/Renode.app/Contents/MacOS/renode-test',
  required: false,
  native: true,
)

if renode_test.found()

  mcu_name  = meson.get_external_property('mcu_name')
  stm32_mcu = meson.get_external_property('stm32_mcu')

  foreach tl : target_tests
    target_exe = executable(
      'test_' + tl + '_target',
      tl + '.cpp',
      dependencies: [ut_dep, hal_dep],
      include_directories: hal_inc,
    )

    resc_conf = configuration_data()
    resc_conf.set('mcu_name',  mcu_name)
    resc_conf.set('repl_path', platform_repl.full_path())
    resc_conf.set('svd_path',  svd_file)
    resc_conf.set('elf_path',  target_exe.full_path())

    test_resc = configure_file(
      input: '../renode/test.resc.in',
      output: 'test_' + tl + '.resc',
      configuration: resc_conf,
    )

    robot_conf = configuration_data()
    robot_conf.set('mcu_name',  mcu_name)
    robot_conf.set('resc_path', test_resc.full_path())
    robot_conf.set('robot_path', meson.current_build_dir() / 'test_' + tl + '.robot')

    test_robot = configure_file(
      input: '../renode/test.robot.in',
      output: 'test_' + tl + '.robot',
      configuration: robot_conf,
    )

    test('test_' + tl + '_renode', renode_test,
      args: [test_robot.full_path()],
      timeout: 60,
    )
  endforeach

endif
