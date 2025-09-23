from SCons.Environment import Environment
from SCons.Script import Glob

base_env = Environment(CPPPATH=['src/shared'])
base_env.VariantDir('build', 'src', duplicate=0)

shared_files = Glob('build/shared/*.cpp')
client_files = Glob('build/client/*.cpp')
server_files = Glob('build/server/*.cpp')

# Client build
client_env = base_env.Clone()
client_env.Append(CPPPATH=['src/client'])
client_env.Program("build/client", client_files + shared_files)

# Server build
server_env = base_env.Clone()
server_env.Append(CPPPATH=['src/server'])
server_env.Program("build/server", server_files + shared_files)