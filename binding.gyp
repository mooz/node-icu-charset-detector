{
  'targets': [
    {
      'target_name': 'node-icu-charset-detector',
      'sources': [ 'node-icu-charset-detector.cpp' ],
      'cflags!': [ '-fno-exceptions', '`icu-config --cppflags`' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'libraries': [ '`icu-config --ldflags`' ],
      'conditions': [
        ['OS=="mac"', {
          'include_dirs': [
              '/opt/local/include'
          ],
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          }
        }]
      ]
    }
  ]
}
