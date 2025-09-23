env = Environment()
env.VariantDir('build', 'src', duplicate=0)

shared_files = Glob('build/shared/*.cpp')
client_files = Glob('build/client/*.cpp')
server_files = Glob('build/server/*.cpp')

# Client build
env.Program("build/client", client_files + shared_files)

# Server build
env.Program("build/server", server_files + shared_files)