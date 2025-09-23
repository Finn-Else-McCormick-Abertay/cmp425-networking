from SCons.Environment import Environment
from SCons.Script import (
    Glob,
    Depends,
)
import os

class Dependency:
    def __init__(self, name, libs = []):
        self.name = name

        if isinstance(libs, str):
            self.libs = [libs]
        else:
            self.libs = libs
        
        self.rootpath = 'lib/' + self.name
        self.libpath = self.rootpath + '/lib'
        self.includepath = self.rootpath + '/include'
        
        dlls = Glob(self.rootpath + '/bin/*.dll')
        if dlls:
            base_env.Install('build/', dlls)
            
        #for lib in self.libs:
        #    relevant_dlls = Glob(self.rootpath + '/bin/' + lib + '*.dll')
        #    if relevant_dlls:
        #        base_env.Install('build/', relevant_dlls)
    
    def add_to_env(self, env):
        if self.libs:
            env.Append(LIBS=self.libs)
        if self.libpath:
            env.Append(LIBPATH=[self.libpath])
        if self.includepath:
            env.Append(CPPPATH=[self.includepath])
    
    def require_for(self, program):
        pass

class LocalDependency(Dependency):
    def __init__(self, folder, env):
        self.name = folder

        self.libs = [self.name]
        self.libpath = 'build/' + folder
        self.includepath = 'src/' + folder
        self.source = Glob(self.libpath + '/*.cpp')
        self.rootpath = ''

        self.add_to_env(env)
        self.lib = env.Library(target=self.libpath + '/' + self.name, source=self.source)
    
    def require_for(self, program):
        Depends(program, self.lib)

# Build configuration logic
def create_program(name, dependencies = []):
    env = base_env.Clone()
    env.Append(CPPPATH=['src/' + name])
    for dependency in dependencies:
        dependency.add_to_env(env)
    program = env.Program(target='build/' + name, source=Glob('build/' + name + '/*.cpp'))
    for dependency in dependencies:
        dependency.require_for(program)
    return program

# -------------

# Setup build environment
base_env = Environment(tools=["mingw"], ENV = os.environ, CC = 'gcc', CCFLAGS = '-O2')
base_env.VariantDir(variant_dir='build', src_dir='src', duplicate=0)

# Build shared files as a library
shared_lib = LocalDependency('shared', base_env)

# SFML dependencies
sfml_graphics = Dependency('SFML-3.0.2', ['sfml-window', 'sfml-graphics'])
sfml_system = Dependency('SFML-3.0.2', 'sfml-system')
sfml_network = Dependency('SFML-3.0.2', 'sfml-network')

# Client build
create_program('client', [shared_lib, sfml_system, sfml_network, sfml_graphics])

# Server build
create_program('server', [shared_lib, sfml_system, sfml_network])