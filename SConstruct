from SCons.Environment import Environment # type: ignore
from SCons.Script import ( Glob, Depends, ) # type: ignore
import os.path
import urllib.request
import zipfile

def GlobCxxFilesIn(folder):
    return Glob(folder + '/*.cpp') + Glob(folder + '/*.cxx') + Glob(folder + '/*.cc')

class Dependency:
    def __init__(self, name, download=None, install = True):
        self.name = name

        self.libs = []
        self.rootpath = 'lib/' + self.name
        self.libpath = self.rootpath + '/lib'
        self.includepath = self.rootpath + '/include'

        if install:
            if not os.path.isdir(self.rootpath):
                print('Dependency \'' + name + '\' not found.')
                if isinstance(download, str): downloadPath = download
                if isinstance(download, dict):
                    repo = download.get("repo")
                    if repo:
                        tag = download.get("tag", "latest")
                        downloadPath = repo + ('/' if not repo.endswith('/') else '') + 'releases/download/' + tag + '/'
                        fileMeta = download.get("file")
                        if fileMeta:
                            if isinstance(fileMeta, str): filePath = fileMeta
                            else:
                                # TODO: Replace with logic to actually get the current platform
                                filePath = fileMeta.get("mingw")
                        if filePath: downloadPath += filePath

                if downloadPath:
                    downloadPath = ('https://' if not downloadPath.startswith(('https://', 'http://')) else '') + downloadPath
                    print('Downloading from \'' + downloadPath + '\'...')
                    filehandle, _ = urllib.request.urlretrieve(downloadPath)
                    if filehandle:
                        print('Download complete. Unzipping...')
                        with zipfile.ZipFile(filehandle, 'r') as zip:
                            zip.extractall('lib/')
                    else:
                        print('Download failed.')
            
            dlls = Glob(self.rootpath + '/bin/*.dll')
            if dlls: base_env.Install('build/', dlls)
        
    def add_to_env(self, env):
        if self.libs:        env.Append(LIBS=self.libs)
        if self.libpath:     env.Append(LIBPATH=[self.libpath])
        if self.includepath: env.Append(CPPPATH=[self.includepath])
    
    def require_for(self, program):
        pass

    def with_libs(self, libs):
        return_dep = Dependency(self.name, install=False)
        if isinstance(libs, str): libs = [libs]
        return_dep.libs = libs
        return return_dep
        

class LocalDependency(Dependency):
    def __init__(self, folder, dependencies=[], env=None):
        self.name = folder

        self.libs = [self.name]
        self.libpath = 'build/' + folder
        self.includepath = 'src/' + folder
        self.source = GlobCxxFilesIn(self.libpath)
        self.rootpath = ''

        if not env: env = base_env
        env = env.Clone()

        for dependency in dependencies: dependency.add_to_env(env)

        self.add_to_env(env)
        self.lib = env.Library(target=self.libpath + '/' + self.name, source=self.source)
        
        for dependency in dependencies: dependency.require_for(self.lib)
    
    def require_for(self, program):
        Depends(program, self.lib)

# Build configuration logic
def create_program(name, dependencies = []):
    env = base_env.Clone()
    env.Append(CPPPATH=['src/' + name])
    for dependency in dependencies:
        dependency.add_to_env(env)
    program = env.Program(target='build/' + name, source=GlobCxxFilesIn('build/' + name))
    for dependency in dependencies:
        dependency.require_for(program)
    return program

# -------------

# Setup build environment
base_env = Environment(tools=["mingw"], ENV = os.environ, CC = 'gcc', CCFLAGS = ['-O2', '-std=c++23']) #'-fmodules', '-include', 'bits/stdc++.h'
base_env.VariantDir(variant_dir='build', src_dir='src', duplicate=0)

# SFML dependencies
sfml = Dependency('SFML-3.0.2', { "repo": "github.com/SFML/SFML/", "tag": "3.0.2", "file": { "mingw": "SFML-3.0.2-Windows.MinGW.x64.zip" } })

sfml_graphics = sfml.with_libs(['sfml-window', 'sfml-graphics'])
sfml_audio = sfml.with_libs('sfml-audio')
sfml_system = sfml.with_libs('sfml-system')
sfml_network = sfml.with_libs('sfml-network')

# Build shared files as a library
shared_lib = LocalDependency('shared', [sfml_network])

# Client build
create_program('client', [shared_lib, sfml_system, sfml_network, sfml_graphics, sfml_audio])

# Server build
create_program('server', [shared_lib, sfml_system, sfml_network])