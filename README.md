# Remote Memory Debugger

Small project written to help search for and change memory values in games.

Server runs on a PC and can be connected to over Netcat etc.

This doesn't work at the moment.

# Usage

    remote_debugger     # start the debugger on the local machine with a local CLI
    remote_debugger -n  # start the debugger with a udp server

By default the remote_debugger runs on port 4444 and uses UDP

To connect to it you can use netcat, i.e.

    nc64 localhost 4444 -u -vv
