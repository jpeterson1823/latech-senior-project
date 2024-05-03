
def init():
    global gCommands
    gCommands = {"command": "", "parsed": ""}

def poll() -> dict:
    global gCommands
    return gCommands

def update(command: str, parsed: str):
    global gCommands
    gCommands["command"] = command
    gCommands["parsed"] = parsed
