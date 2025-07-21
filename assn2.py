"""
Write a minimal version of a shell. The shell should be able to:
a) execute a program without the complete path name
b) handle pipes
c) handle redirection
d) handle signals
"""
import shlex
import subprocess

import shlex
import subprocess

def parse_pipeline(cmd):       # STEP 2
    """Step 1: Parse the pipeline command into individual command parts."""
    return [shlex.split(part.strip()) for part in cmd.split('|')]

def execute_pipeline(commands):     # STEP 2
    """Step 2: Execute a list of commands as a pipeline."""
    prev_proc = None
    for i, part in enumerate(commands):
        if i == 0:
            proc = subprocess.Popen(part, stdout=subprocess.PIPE)
        elif i == len(commands) - 1:
            proc = subprocess.Popen(part, stdin=prev_proc.stdout)
        else:
            proc = subprocess.Popen(part, stdin=prev_proc.stdout, stdout=subprocess.PIPE)

        if prev_proc:
            prev_proc.stdout.close()
        prev_proc = proc

    proc.communicate()

def run_command(cmd):
    if "|" in cmd:
        commands = parse_pipeline(cmd)
        execute_pipeline(commands)
    else:       # STEP 1
        args = shlex.split(cmd)
        try:
            subprocess.run(args)
        except FileNotFoundError:
            print("invalid command")
        except Exception as e:
            print(f"Error: {e}")

while True:
    cmd = input("Enter the command: ")
    if cmd.lower() == "exit":
        print("Chal Bye")
        break
    if cmd.strip():
        run_command(cmd)

    