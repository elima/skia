# Build ALMOST everything provided by Skia; this should be the default target.
#
# This omits the following targets that many developers won't want to build:
# - debugger: this requires QT to build
#
{
  'variables': {
    'skia_skip_gui%': 0,
  },
  'targets': [
    {
      'target_name': 'most',
      'type': 'none',
      'dependencies': [
        # The minimal set of static libraries for basic Skia functionality.
        'skia_lib.gyp:skia_lib',

        'bench.gyp:bench',
        'gm.gyp:gm',
        'SampleApp.gyp:SampleApp',
        'tests.gyp:tests',
        'tools.gyp:tools',
        'pathops_unittest.gyp:*',
        'skpskgr_test.gyp:*',
#       'pdfviewer.gyp:pdfviewer',
        'dm.gyp:dm',
        'perf.gyp:perf'
      ],
      'conditions': [
        ['skia_os == "android"', {
          'dependencies': [ 'android_system.gyp:SampleApp_APK' ],
        }],
        ['skia_os == "ios"', {
          'dependencies!': [ 'SampleApp.gyp:SampleApp' ],
        }],
        [ 'skia_skip_gui',
          {
            'dependencies!': [
              'SampleApp.gyp:SampleApp',
              'skpskgr_test.gyp:*',
            ]
          }
        ]
      ],
    },
  ],
}
