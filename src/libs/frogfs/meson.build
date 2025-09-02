project('frogfs', 'c')

frogfs_includes = include_directories('include')
frogfs_sources = files(
    'src' / 'decomp_raw.c',
    'src' / 'frogfs.c',
)
frogfs_defines = []
frogfs_deps = []

log_level = get_option('log-level')
if log_level == 'none'
    frogfs_defines += '-DCONFIG_FROGFS_LOG_LEVEL_NONE=1'
elif log_level == 'error'
    frogfs_defines += '-DCONFIG_FROGFS_LOG_LEVEL_ERROR=1'
elif log_level == 'warn'
    frogfs_defines += '-DCONFIG_FROGFS_LOG_LEVEL_WARN=1'
elif log_level == 'info'
    frogfs_defines += '-DCONFIG_FROGFS_LOG_LEVEL_INFO=1'
elif log_level == 'debug'
    frogfs_defines += '-DCONFIG_FROGFS_LOG_LEVEL_DEBUG=1'
elif log_level == 'verbose'
    frogfs_defines += '-DCONFIG_FROGFS_LOG_LEVEL_VERBOSE=1'
endif

if get_option('use-heatshrink')
    heatshrink_dep = dependency('heatshrink')
    frogfs_sources += files(
        'src' / 'decomp_heatshrink.c',
    )
    frogfs_defines += '-DCONFIG_FROGFS_USE_HEATSHRINK=1'
    frogfs_deps += heatshrink_dep
endif

if get_option('use-miniz')
    miniz_dep = dependency('miniz')
    frogfs_sources += files(
        'src' / 'decomp_miniz.c',
    )
    frogfs_defines += '-DCONFIG_FROGFS_USE_MINIZ=1'
    frogfs_deps += miniz_dep
endif

if get_option('use-zlib')
    zlib_dep = dependency('libz', required: true)
    frogfs_sources += files(
        'src' / 'decomp_zlib.c',
    )
    frogfs_defines += '-DCONFIG_FROGFS_USE_ZLIB=1'
    frogfs_deps += zlib_dep
endif

libfrogfs = static_library('frogfs',
    frogfs_sources,
    dependencies: frogfs_deps,
    include_directories: frogfs_includes
)

frogfs_dep = declare_dependency(
    link_with: libfrogfs,
    include_directories: frogfs_includes
)

meson.override_dependency('frogfs', frogfs_dep)

bin2c_py = find_program('tools' / 'bin2c.py')
mkfrogfs_py = find_program('tools' / 'mkfrogfs.py')

# silence language server unused variable warnings
bin2c_py = bin2c_py
mkfrogfs_py = mkfrogfs_py
