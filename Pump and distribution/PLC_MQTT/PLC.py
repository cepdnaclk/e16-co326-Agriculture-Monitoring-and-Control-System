from pymodbus.client.sync import ModbusTcpClient

class PLC:
    def __init__(self, ip = '127.0.0.1'):
        self.client = ModbusTcpClient(ip)
        self.debug = False
        self.pump = 1   
        self.gh = 0
        self.hp = 4
        self.tank_l = 2
        self.tank_h = 3
        self.res_l = 0
        self.res_h = 1
            
    def control_pump(self, state):
        #  TODO:config the coil number
        if self.debug: return
        self.client.write_coil(self.pump, state)

    def control_distrib_pump(self, idx, state):
        #  TODO:config the coil number
        if self.debug: return
        self.client.write_coil(idx*4, state)

    def get_tank_lvl(self):
        if self.debug: return 10
        #  TODO:config the coil number
        lvl1 = self.client.read_discrete_inputs(0,8).bits[self.tank_l]
        lvl2 = self.client.read_discrete_inputs(0,8).bits[self.tank_h]

        if lvl1 == 0 and lvl2 == 0:
            return 0
        if lvl1 == 1 and lvl2 == 0:
            return 50
        if lvl1 == 1 and lvl2 == 1:
            return 100

    def get_resv_lvl(self):
        if self.debug: return 10
        #  TODO:config the coil number
        lvl1 = self.client.read_discrete_inputs(0,8).bits[self.res_l]
        lvl2 = self.client.read_discrete_inputs(0,8).bits[self.res_h]
        
        if lvl1 == 0 and lvl2 == 0:
            return 0
        if lvl1 == 1 and lvl2 == 0:
            return 50
        if lvl1 == 1 and lvl2 == 1:
            return 100
    def get_pumps(self):
        if not self.debug: 
            #  TODO:config the coil number
            p  = self.client.read_coils(0,8).bits[self.pump]
            gh = self.client.read_coils(0,8).bits[self.gh]
            hp = self.client.read_coils(0,8).bits[self.hp]
        else:
            p = 1
            gh = 0
            hp = 1
        states = [ "ON" if p else "OFF", "ON" if gh else "OFF", "ON" if hp else "OFF"]
        return states
