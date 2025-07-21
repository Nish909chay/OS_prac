"""
Write a minimal version of a shell. The shell should be able to:
a) execute a program without the complete path name
b) handle pipes
c) handle redirection
d) handle signals
"""
import shlex
import subprocess

def run_command(cmd):
    # step 1 - parse the pipeline
    if "|" in cmd:      # "ls -l | grep txt | sort"
        parts = [shlex.split(part.strip()) for part in cmd.split('|')] # ['ls', '-l']
        prev_proc = None
        
        for i, part in enumerate(parts):
            if i == 0:
                proc = subprocess.Popen(part, stdout = subprocess.PIPE)
            elif(i == len(parts) - 1):
                proc = subprocess.Popen(part, stdin = prev_proc.stdout)
            else:
                proc = subprocess.Popen(part, stdin = prev_proc.stdout, stdout = subprocess.PIPE)
            if prev_proc:
                prev_proc.stdout.close()
            prev_proc = proc       
        proc.communicate()        
            
    else:   # no pipelined | command
        args = shlex.split(cmd)
        try:
            subprocess.run(args)
        except FileNotFoundError:   # invalid commands
            print("invalid command")
        except Exception as e:
            print(f"Error : {e}")
    
    
while True:
    print("Enter the command : ", )
    cmd = input()
    
    if(cmd.lower() == "exit"):
        print("Chal Bye")
        break
    
    if cmd.strip():
        run_command(cmd)
    