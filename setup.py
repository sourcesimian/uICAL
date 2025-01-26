from distutils.core import setup, Extension
import glob

_DEBUG = True

_DEBUG_LEVEL = 1
_UICAL_LOG_LEVEL = 5


extra_compile_args = ["-std=c++11", "-Wall", "-Wextra", "-Wno-missing-field-initializers"]
if _DEBUG:
    extra_compile_args += ["-g3", "-O0", "-DDEBUG=%s" % _DEBUG_LEVEL, "-UNDEBUG", "-DUICAL_LOG_LEVEL=%s" % _UICAL_LOG_LEVEL]
else:
    extra_compile_args += ["-DNDEBUG", "-O3"]

# extra_compile_args += ["--coverage", "-lgcov"]

# extra_compile_args += ["-ftest-coverage", "-fprofile-arcs"]


uical = Extension(
    'uICAL',
    language = 'c++',
    # define_macros = [('MAJOR_VERSION', '1'),
    #                  ('MINOR_VERSION', '0')],
    include_dirs = ['./src'],
    # libraries = ['tcl83'],
    # library_dirs = ['/usr/local/lib'],
    sources = glob.glob('./python-module/*.cpp') + glob.glob('./src/*.cpp'),
    extra_compile_args = extra_compile_args,
)

setup (
    name = 'uICAL',
    version = '0.0.2',
    description = 'Light weight ICAL library',
    author = 'Source Simian',
    author_email = 'source.simian@gmail.com',
    url = 'https://github.com/sourcesimian/uical',
    long_description = '''
This is really just a demo package.
''',
    ext_modules = [uical],
    # test_require = [
    #     'pytest',
    #     'pytest-faulthandler',
    # ],
)
