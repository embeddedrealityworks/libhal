native_tests = ['t1']
target_tests = ['t2']

foreach tl : native_tests
  tests_name = 'test_' +tl
  test_exe = executable(
    test_name,
    tl + '.cpp',
    depepndencies : [ut_dep, hal_dep],
    native: true
    )
  test(test_name, test_exe)
endforeach
