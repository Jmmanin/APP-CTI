# testing the output of the DPS client live with the server

from DPSComm import DPS_interface
import time

count = 0
dps = DPS_interface()
dps.select_mode('l')

start_t = time.time()
while (time.time() - start_t) < 15:
    (pkt,resp) = dps.get_live_packet()
    if resp != 1:
        print('Got packet! ')
        count += 1

dps.release_mode('k')
print('packets recieved: ' + str(count))