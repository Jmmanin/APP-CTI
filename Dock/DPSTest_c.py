# client for testing DPSComm

from DPSComm import DPS_interface
import time

def main():
    dps = DPS_interface()
    count = 0
    loop = 0

    while loop < 2:
        '''dps.select_mode("l")
        while count < 8:
            (pkt, resp) = dps.get_live_packet()
            if pkt is not None:
                print("packet["+ str(count) + "] resp: " + str(resp) + " pkt: '" + pkt.decode('ascii') + "'")
            count += 1
        
        count = 0
        dps.release_mode()
        time.sleep(2)
        if(dps.select_mode("p") == 2):'''
        dps.select_mode("p")
        
        while count <= 6:
            (stream, resp) = dps.get_recorded_run(count)
            print("stream["+ str(count) + "] resp: " + str(resp) + " pkt: '" + stream.decode('ascii') + "'")
        
        count = 0
        dps.release_mode()


if __name__ == "__main__":
    main()