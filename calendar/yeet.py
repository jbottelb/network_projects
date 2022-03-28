#!/usr/bin/env python3

import os

for _ in range(5):
    os.system("./client.py JoeC add date 022322 time 1100 duration 60 name \"Quiz\" description \"CSE 30264 Quiz Due\"")

for i in range(2, 3):
    os.system(f"./client.py JoeC remove {i}")

os.system("./client.py JoeC update 0 time 2321")
os.system("./client.py JoeC update 1 date 112321")

os.system("./client.py JoeC get 112321")
os.system("./client.py JoeC get 022322")

os.system("./client.py JoeC getall 022320 112322")





