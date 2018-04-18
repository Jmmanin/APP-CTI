'''
Wear Function class for analyzing output of the nueral network
Each Class contains the wear function values, as well as a memory array
for each of the time steps.

'''

class WearFunction():

    # memory and state elements
    timestep_memory = list()
    curr_tstep = 0
    w_c = 0
    ttl = 300
    active = 0
    num_tsteps = 0
    
    # constants
    sample_rate = 1
    T_recov = 10
    t_rstart = 0
    Max_seconds = 300


    def __init__(self, sampleRate=1, initalArray=None):
        if initalArray is not None:
            self.timestep_memory = initalArray
        self.sample_rate = sampleRate
		
    def get_current(self):
        return (self.w_c, self.ttl, self.curr_tstep, self.active)

    def get_current_wear(self):
        return self.w_c

    def get_current_ttl(self):
        return self.ttl

    def calc_next_wear(self, w):
        
        # steps for if active during timestep
        if w > 1 :
            w_new = w / self.sample_rate
            self.active = 1
            self.w_c += w_new
            self.ttl = (self.Max_seconds - self.w_c) / w
            if self.ttl < 0 :
                self.ttl = 0

        # steps for if resting during timestep
        else:
            if self.active == 1:
                self.t_rstart = self.curr_tstep
                self.active = 0
            self.w_c *= 1 - ((self.curr_tstep + 1 - self.t_rstart) / (self.sample_rate * self.T_recov))
            # ^ w_c zeros at T_recover so the junk generated after is ok 
            self.ttl = self.Max_seconds - self.w_c

        store_tuple = (self.w_c, self.ttl, (self.curr_tstep/self.sample_rate), self.active)
        self.timestep_memory.append(store_tuple)
        self.num_tsteps += 1
        return store_tuple

    def peek_at_timestep(self, timeslice, mode='i'):
        new_pos = None
        if mode == 's':
            timeslice = timeslice / self.sample_rate
            if timeslice >= self.num_tsteps :
                new_pos = self.timestep_memory[int(timeslice)]
        else :
            new_pos = self.timestep_memory[timeslice]

        return new_pos
		
    def get_recorded_history(self):
        return self.timestep_memory