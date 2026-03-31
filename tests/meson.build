native_tests = ['units']

foreach tl : native_tests
  test_name = 'test_' +tl
  test_exe = executable(
    test_name,
    tl + '.cpp',
    dependencies : [ut_dep],
    include_directories: hal_inc,
    native: true
    )
  test(test_name, test_exe)
endforeach
