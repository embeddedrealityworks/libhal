doxygen = find_program('doxygen', required: false)
dot = find_program('dot', required: false)
asciidoc = find_program('asciidoctor', required: false)

alldocs = []

if doxygen.found()
  src = meson.project_source_root()
  doxy_input = [src / 'include', src / 'src']

  cdata = configuration_data()
  cdata.set('PROJECT_NAME', meson.project_name())
  cdata.set('PROJECT_VERSION', meson.project_version())
  cdata.set('PROJECT_BRIEF', 'HAL Library')
  cdata.set('PROJECT_BUNDLE_ID', 'com.embeddedreality.libhal')
  cdata.set('PROJECT_PUBLISHER_ID', 'com.embeddedreality')
  cdata.set('PROJECT_INPUT', ' '.join(doxy_input))
  if dot.found()
    cdata.set('PROJECT_HAVE_DOT', 'YES')
    cdata.set('PROJECT_DOT_PATH', dot.full_path())
  else
    cdata.set('PROJECT_HAVE_DOT', 'NO')
    cdata.set('PROJECT_DOT_PATH', '')
  endif

  doxyfile = configure_file(
      input: 'Doxyfile.in',
      output: 'Doxyfile',
      configuration: cdata,
      install: false,
    )

  alldocs += custom_target(
    'api',
    input: doxyfile,
    output: 'doxygen',
    command: [doxygen, '@INPUT@'])

else
  message('Doxygen not found, API documentation will not be built')
endif

if asciidoc.found()
  adoc_files = files('index.adoc')

  alldocs += custom_target(
    'adocs',
    command: [
      asciidoc,
      '-b',
      'html5',
      '-d',
      'book',
      '--failure-level',
      'ERROR',
      '-r',
      'asciidoctor-diagram',
      '-a',
      'release-version=@0@'.format(meson.project_version()),
      '@INPUT@',
      '-D',
      '@OUTPUT@',
      ],
    input: adoc_files,
    output: 'manual'
    )
else
  message('Asciidoctor not found, documentation will not be built')
endif

run_target('docs', command: ['echo', 'Built docs'], depends: alldocs)
