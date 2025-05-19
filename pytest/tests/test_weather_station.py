import os
import time
import subprocess
import pytest

import time
from test_config import test_cases

def parse_display(filepath="display.txt"):
    with open(filepath, "r") as f:
        lines = f.readlines()
        
    t = lines[1].removeprefix("Time: ").removesuffix("\n")
    t = time.strptime(t, r"%Y-%m-%d %H:%M:%S")

    readings = lines[2].removeprefix("Readings: ").removesuffix("\n").split()
    readings = {"Temperature": float(readings[0]), "Humidity": float(readings[2]), "Pressure": float(readings[4]), "Wind Speed": float(readings[6]), "Wind Direction": float(readings[8])}
    return (t, readings)
        

@pytest.mark.parametrize("test_case", test_cases)
def test_weather_station(test_case):
    # Cleanup before test
    for f in ["display.txt", "storage.txt"]:
        if os.path.exists(f):
            os.remove(f)

    # Start process
    proc = subprocess.Popen(["./src/main.exe"], stdin=subprocess.PIPE)
    time.sleep(2)
    proc.stdin.write(test_case["input"].encode())
    proc.stdin.flush()
    time.sleep(2)
    proc.kill()

    if not os.path.exists("display.txt"):
        pytest.fail("display.txt was not created")

    timestamp, readings = parse_display()
    expectedTime = time.localtime()

    for check in test_case["checks"]:
        if check["type"] == "range":
            value = readings[check["field"]]
            assert check["min"] <= value <= check["max"], f"{test_case['id']}: {check['field']}={value} not in range {check['min']}–{check['max']}"
        elif check["type"] == "file_exists":
            assert os.path.exists(check["file"]) and os.path.getsize(check["file"]) > 0, f"{test_case['id']}: File '{check['file']}' missing or empty"
        elif check["type"] == "time_within":
            now = expectedTime
            dt = abs(time.mktime(now) - time.mktime(timestamp))
            assert dt <= check["tolerance"], f"{test_case['id']}: Timestamp offset too large ({dt:.2f}s)"
    
    #clean up temporary files
    for f in ["display.txt", "storage.txt"]:
        if os.path.exists(f):
            os.remove(f)