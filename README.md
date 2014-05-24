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
    
# Todo

1. Get it working - can only modify own process (i.e. the debugger!), might be permissions
2. Convert from hex on the mm (memory modify) command
3. Testing
    
# Commands

## Listing all the processes running

    lp 
    >> Process List
    Process 464, Steam.exe,
    Process 1268, WDExpress.exe,
    Process 1520, remote_debugger.exe,
    Process 2232, MSBuild.exe,
    Process 2732, tposdsvc.exe,
    Process 2924, chrome.exe,
    Process 3128, SCHTASK.exe,
    Process 3176, TPONSCR.exe,
    Process 3208, TpScrex.exe,
    Process 3512, TpKnrres.exe,
    Process 3700, rundll32.exe,
    Process 3916, vpnui.exe,
    Process 3928, iTunesHelper.exe,
    Process 4484, chrome.exe,
    Process 4708, chrome.exe,
    Process 4808, chrome.exe,
    Process 4912, chrome.exe,
    Process 4960, chrome.exe,
    Process 5104, pageant.exe,
    Process 5168, VCPkgSrv.exe,
    Process 5468, VCPkgSrv.exe,
    Process 6076, MSBuild.exe,
    
## Detailed list of processes

    dl
    >> Process List
    Process 464, Steam.exe,
            SizeOfImage:            00232000
            EntryPoint:             00A6C00B
            BaseAddressOfDLL:       009D0000
            Working Memory:         01C29000
            Page File Usage:        04576000
    Process 1268, WDExpress.exe,
            SizeOfImage:            00077000
            EntryPoint:             00FF7F79
            BaseAddressOfDLL:       00FC0000
            Working Memory:         1279B000
            Page File Usage:        0B3B0000
            
## Detailed info for a given Pid

    di 1520
    Process 1520, remote_debugger.exe, C:\User...ug\remote_debugger.exe
            SizeOfImage:            00046000
            EntryPoint:             013BEAA0
            BaseAddressOfDLL:       01390000
            Working Memory:         00474000
            Page File Usage:        00142000
            
## Debugging a process

    dp 1520
    >> Debugging process 1520 returned: 1
    
## Searching for a memory value in a Pid

    sm 1520, 4, 0
    Search TxId: 4347
    Search has 51414 results

The search transaction id is needed to refine the search so don't lose it

## Refining a search result

    rs 4347, 1
    Search has 3984 results
    
## Listing the search results

    lsr 4347
        0047FFDC
        0047FFE0
        0047FFE4
        0047FFE8
        0047FFEC
        ...
      
You can keep track by using the monitor command

    ms4347
            0x00100160 = 0x00000001 (1)
            0x0038B25C = 0x00000001 (1)
            0x0038F398 = 0x00000001 (1)
    x to exit, others to contine
    
## Modifying a value

    mm 1520,1048928,4,0
    Memory modified OK: 0x00100160 to 0x00000000
    
Yes, you need to convert from hex (addr) to dec.

We can now check it worked with the monitor

    ms4347
        0x00100160 = 0x00000000 (0)
        0x0038B25C = 0x00000001 (1)
        0x0038F398 = 0x00000001 (1)
