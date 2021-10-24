from pymodbus.client.sync import ModbusTcpClient

class PLC:
    def __init__(self, ip = '127.0.0.1'):
        self.client = ModbusTcpClient(ip)
    
    def control_pump(self, state):
        #  TODO:config the coil number
        self.client.write_coil(1, state)

    def control_distrib_pump(self, idx, state):
        #  TODO:config the coil number
        self.client.write_coil(idx, state)
