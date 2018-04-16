# client for testing DPSComm

# note it is important to call things in the right order to keep the server and client in sync
''' 
Set up connection:
    dps = DPS_Interface()
Select a mode:
    dps.select_mode('mode')  <= 'l' live stream packets, 'p' prdat arrays
Call mode function:
    if 'l':
        dps.get_live_packet()
    if 'p':
        dps.get_recorded_run()
Switch context:
    dps.release_mode() <= 'r' (default) sends server back into context-wait mode (call .select_mode)
                       <= 'k' release mode and close connection entirely
                       <= 'n' (internally used) check but do not release mode. (you don't need to use this one)
'''

from DPSComm import DPS_interface
import time

def main():
    dps = DPS_interface()
    count = 1
    loop = 0

    while loop < 2:
        #dps = DPS_interface()
        dps.select_mode('l')        # selects accept live packet mode
        while count < 16:
            (pkt, resp) = dps.get_live_packet() # gets a single packet being queued, times out and gives None if packet is unavailable
            if pkt is not None:
                print("packet["+ str(count) + "] resp: " + str(resp) + " pkt: '" + pkt.decode('ascii') + "'")
            else:
                print("packet[" + str(count) + "] - queue is empty")
            count += 1
        
        count = 0
        dps.release_mode()  # releases the livestream context. Server is waiting for a new mode cmd but connection is still up
        time.sleep(2)
        #dps = DPS_interface()
        dps.select_mode('p')    # selects pre-recorded stream mode
        
        while count <= 6:
            (stream, resp) = dps.get_recorded_run(count)    # when given a stream id as input, will pull the entire stream array back in, None if the id doesn't match a record
            if(resp == 0):
                print("stream["+ str(count) + "] resp: " + str(resp) + " pkt: '" + stream.decode('ascii') + "'")
            else:
               print("stream["+ str(count) + "] resp: " + str(resp) + " pkt: '-No response-'") 
            count += 1
        
        count = 0
        loop += 1
        dps.release_mode()
    dps.select_mode('l')    # bc mode was released in the loop, calling .release_mode('k') becomes an out-of-sync call. I gave it a new mode
                            # so that I can tell the server to release and close. A wierd quirk but the server cannot close the connection while
                            # waiting for context
    dps.release_mode('k')  # the close down function. Releases connection in entirity


if __name__ == "__main__":
    main()