import shlex
cmd = "ls -l | grep txt"
args = str.split(" | ")

if '|' in cmd:
    parts = [part.strip() for part in cmd.split('|')]
    print("Pipeline commands:")
    for i, part in enumerate(parts, start=1):
        print(f"Command {i}:", part)