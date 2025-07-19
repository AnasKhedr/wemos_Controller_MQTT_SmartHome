import subprocess
import os
Import("env")

print("Current CLI targets", COMMAND_LINE_TARGETS)
print("Current Build targets", BUILD_TARGETS)

# include toolchain paths
env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

# override compilation DB path
env.Replace(COMPILATIONDB_PATH=os.path.join("$BUILD_DIR", "../compile_commands.json"))

print('cwd: ',os.getcwd())
print('COMPILATIONDB_PATH: ', env.get('COMPILATIONDB_PATH', 'Not set'))

if "compiledb" not in COMMAND_LINE_TARGETS: #avoids infinite recursion
    print('generating compile commands.')
    subprocess.run(['pio', 'run','-t','compiledb', '-v'])
    print('Done: generating compile commands.')