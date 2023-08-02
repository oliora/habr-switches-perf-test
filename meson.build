project(
    'habr-switches-perf-test',
    ['cpp'],
    version : '0.1.0',
    default_options : [
        'cpp_std=c++2a',
        'default_library=static',
        'warning_level=2',
        'werror=true',
        'buildtype=release',
	'debug=true',
        'b_lto=true',
        'b_ndebug=true',
    ]
)

add_global_arguments(['-ffast-math'], language: ['c', 'cpp'])

architecture = get_option('architecture')
summary({'architecture': architecture})

if architecture == 'native'
    arch_flags = [
        '-march=native',
        '-mtune=native',
        '-mavx',
        '-mavx2',
        '-mno-avx256-split-unaligned-load',
        '-msse',
        '-msse2',
        '-msse3',
    ]
elif architecture == 'icelake'
    arch_flags = [
        '-march=icelake-server',
        '-mmmx',
        '-mno-3dnow',
        '-msse',
        '-msse2',
        '-msse3',
        '-mssse3',
        '-mno-sse4a',
        '-mcx16',
        '-msahf',
        '-mmovbe',
        '-maes',
        '-msha',
        '-mpclmul',
        '-mpopcnt',
        '-mabm',
        '-mno-lwp',
        '-mfma',
        '-mno-fma4',
        '-mno-xop',
        '-mbmi',
        '-msgx',
        '-mbmi2',
        '-mpconfig',
        '-mwbnoinvd',
        '-mno-tbm',
        '-mavx',
        '-mavx2',
        '-msse4.2',
        '-msse4.1',
        '-mlzcnt',
        '-mrtm',
        '-mhle',
        '-mrdrnd',
        '-mf16c',
        '-mfsgsbase',
        '-mrdseed',
        '-mprfchw',
        '-madx',
        '-mfxsr',
        '-mxsave',
        '-mxsaveopt',
        '-mavx512f',
        '-mno-avx512er',
        '-mavx512cd',
        '-mno-avx512pf',
        '-mno-prefetchwt1',
        '-mclflushopt',
        '-mxsavec',
        '-mxsaves',
        '-mavx512dq',
        '-mavx512bw',
        '-mavx512vl',
        '-mavx512ifma',
        '-mavx512vbmi',
        '-mno-avx5124fmaps',
        '-mno-avx5124vnniw',
        '-mclwb',
        '-mno-mwaitx',
        '-mno-clzero',
        '-mpku',
        '-mrdpid',
        '-mgfni',
        '-mno-shstk',
        '-mavx512vbmi2',
        '-mavx512vnni',
        '-mvaes',
        '-mvpclmulqdq',
        '-mavx512bitalg',
        '-mavx512vpopcntdq',
        '-mno-movdiri',
        '-mno-movdir64b',
        '-mno-waitpkg',
        '-mno-cldemote',
        '-mno-ptwrite',
        '-mno-avx512bf16',
        '-mno-enqcmd',
        '-mno-avx512vp2intersect',
        '--param',
        'l1-cache-size=48',
        '--param',
        'l1-cache-line-size=64',
        '--param',
        'l2-cache-size=55296',
        '-mtune=icelake-server',
    ]
endif

add_global_arguments(arch_flags, language: ['c', 'cpp'])

algos = [
    'naive',
    'naiveLessBranches',
    'naiveTableChar',
    'naiveTableInt',
    'autoVec_32',
    'autoVec_64',
    'autoVec_128_WithOverflow',
    'autoVec_128',
    'autoVec_128_IntStepCounter',
    'autoVec_256',
    'autoVec_256_IntStepCounter',
    'autoVec_512',
    'autoVec_1024',
    'autoVec_2048',

    'manualVec_32',
    'manualVec_64',
    'manualVec_128',
    'manualVec_256',
    'manualVec_512',
    'manualVec_1024',
    'manualVec_2048',

    'manualVecSize_32',
    'manualVecSize_64',
    'manualVecSize_128',
    'manualVecSize_256',
    'manualVecSize_512',
    'manualVecSize_1024',
    'manualVecSize_2048',

    'manualVecStrlen_32',
    'manualVecStrlen_64',
    'manualVecStrlen_128',
    'manualVecStrlen_256',
    'manualVecStrlen_512',
    'manualVecStrlen_1024',
    'manualVecStrlen_2048',
]


foreach algo: algos
    name = algo.replace('_', '-')

    executable('algo-' + name, [
            'main.cpp',
        ],
        cpp_args: ['-DTEST_ALGO=' + algo]
    )

    test_exec = executable('test-' + name, [
            'test_main.cpp',
        ],
        cpp_args: ['-DTEST_ALGO=' + algo]
    )

    test('test-' + algo, test_exec, workdir: meson.current_source_dir() + '/input')
endforeach
