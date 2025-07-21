"""
Write a minimal version of a shell. The shell should be able to:
a) execute a program without the complete path name
b) handle pipes
c) handle redirection
d) handle signals
"""
import shlex
import subprocess
import os
import sys
import signal

def signal_handler(signum, frame):
    print()
    print("MiniShell> ", end='', flush=True)
signal.signal(signal.SIGINT, signal_handler)

def parse_pipeline(cmd):       # STEP 2
    """Step 1: Parse the pipeline command into individual command parts."""
    return [shlex.split(part.strip()) for part in cmd.split('|')]

def execute_pipeline(pipeline):
    num_cmds = len(pipeline)
    processes = []
    prev_pipe = None

    for i, cmd in enumerate(pipeline):
        # Handle redirection for the current command
        input_file = None
        output_file = None
        append = False

        if '<' in cmd:
            parts = cmd.split('<')
            cmd = parts[0].strip()
            input_file = parts[1].strip()

        if '>>' in cmd:
            parts = cmd.split('>>')
            cmd = parts[0].strip()
            output_file = parts[1].strip()
            append = True
        elif '>' in cmd:
            parts = cmd.split('>')
            cmd = parts[0].strip()
            output_file = parts[1].strip()
            append = False

        args = cmd

        stdin = prev_pipe
        stdout = subprocess.PIPE if i < num_cmds - 1 else None

        if input_file:
            stdin = open(input_file, 'r')

        if output_file:
            mode = 'a' if append else 'w'
            stdout = open(output_file, mode)

        p = subprocess.Popen(args, stdin=stdin, stdout=stdout)

        if prev_pipe:
            prev_pipe.close()

        # close file if redirection was done
        if input_file and stdin != subprocess.PIPE:
            stdin.close()

        if output_file and stdout != subprocess.PIPE:
            stdout.close()

        prev_pipe = p.stdout
        processes.append(p)

    for p in processes:
        p.wait()


def run_command(cmd):
    if "|" in cmd:
        commands = parse_pipeline(cmd)
        execute_pipeline(commands)
    else:       # STEP 1
        args, stdin_file, stdout_file, append_mode = redirection(cmd)
        stdin = open(stdin_file, 'r') if stdin_file else None
        mode = 'a' if append_mode else 'w'
        stdout = open(stdout_file, mode) if stdout_file else None
        
        try:
            subprocess.run(args, stdin = stdin, stdout = stdout)
        except FileNotFoundError:
            print("invalid command")
        except Exception as e:
            print(f"Error: {e}")
        finally:
            if stdin: stdin.close()
            if stdout: stdout.close()
            
def redirection(cmd):   # Step 3
    stdin_file = None
    stdout_file = None
    append_mode = False
    
    tokens = shlex.split(cmd)
    command = []
    i = 0
    
    while i < len(tokens):
        if tokens[i] == ">":     # output redirection
            stdout_file = tokens[i+1]
            i += 2
        elif tokens[i] == "<":   # input redirection
            stdin_file = tokens[i+1]
            i += 2
        elif(tokens[i] == ">>"):  # output append
            stdout_file = tokens[i+1]
            append_mode = True
            i += 2
        else:
            command.append(tokens[i])
            i += 1
    
    return command, stdin_file, stdout_file, append_mode
        
            
        
    

while True:
    print()
    try:
        cmd = input("Enter the command: ")
    except KeyboardInterrupt:
        print()
        continue
    if cmd.lower() == "exit":
        print("Chal Bye")
        break
    if cmd.strip():
        run_command(cmd)

    