import time

file = open("templeate.txt")

text = ""

def show(pump, tlvl, rlvl, gh, hyp, warn):
    print("\033c")
    tlvl = str(tlvl)+ "%"
    rlvl = str(rlvl)+ "%"
    print(text%(pump+" "*(32-len(pump)),\
          tlvl+" "*(32-len(tlvl)), \
          rlvl+" "*(32-len(rlvl)), \
          gh+" "*(23-len(gh)), \
          hyp+" "*(23-len(hyp)), \
          warn+" "*(44-len(warn))))    


for line in file:
    text = text + line

