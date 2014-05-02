{
  'targets': [
    {
      'target_name': 'perf',
      'type': 'none',
      'dependencies': [
        'msaa-circles',
        'msaa-rrect'
      ]
    },
    {
      'target_name': 'msaa-circles',
      'type': 'executable',
      'include_dirs' : [
        '../include/core',
        '../include/config',
        '../include/effects',
        '../include/images',
        '../include/gpu',
        '../include/gpu/gl',
	'../src/images',
      ],
      'sources': [
        '../perf/msaa-circles.cpp',
      ],
      'dependencies': [
        'skia_lib.gyp:skia_lib',
        'effects.gyp:effects',
        'images.gyp:images',
        'views.gyp:views',
      ],
      'ldflags': [
	'-lglut',
	'-lGLU',
	'-lGLEW',
	'-lglfw',
      ],
    },
    {
      'target_name': 'msaa-rrect',
      'type': 'executable',
      'include_dirs' : [
        '../include/core',
        '../include/config',
        '../include/effects',
        '../include/images',
        '../include/gpu',
        '../include/gpu/gl',
	'../src/images',
      ],
      'sources': [
        '../perf/msaa-rrect.cpp',
      ],
      'dependencies': [
        'skia_lib.gyp:skia_lib',
        'effects.gyp:effects',
        'images.gyp:images',
        'views.gyp:views',
      ],
      'ldflags': [
	'-lglut',
	'-lGLU',
	'-lGLEW',
	'-lglfw',
      ],
    },

  ],
}
