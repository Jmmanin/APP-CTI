# testing the output of the DPS client live with the server

from DPSComm import DPS_interface
import time

def main():
    count = 0
    try:
        dps = DPS_interface()
    except ConnectionRefusedError: 
        print("Failed to link to server. Shutting down.")
        return
    dps.select_mode('l')

    start_t = time.time()
    while (time.time() - start_t) < 5:
        (pkt, resp) = dps.get_live_packet()
        if resp != 1:
            print('Got packet! ')
            count += 1

    dps.release_mode('k')
    print('packets recieved: ' + str(count))


if __name__ == '__main__':
    main()
