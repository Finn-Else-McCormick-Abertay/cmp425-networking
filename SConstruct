from SCons.Environment import Environment
from SCons.Script import (
    Glob,
    Depends
)

project_name = 'coursework'

base_env = Environment()
base_env.VariantDir('build', 'src', duplicate=0)

# Build shared files as a library
shared_lib_name = project_name + "_shared_internal"
base_env.Append(CPPPATH=['src/shared'])
shared_lib = base_env.Library(target='build/' + shared_lib_name, source=Glob('build/shared/*.cpp'))
base_env.Append(LIBS=[shared_lib_name], LIBPATH=['build/'])

# Build configuration logic
def create_program(name, libs = [], lib_paths = []):
    env = base_env.Clone()
    env.Append(CPPPATH=['src/' + name], LIBS=libs, LIBPATH=lib_paths)
    program = env.Program(target='build/' + name, source=Glob('build/' + name + '/*.cpp'))
    Depends(program, shared_lib)
    return program

# Client build
create_program('client')

# Server build
create_program('server')