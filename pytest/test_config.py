test_cases = [
    {
        "id": "TC001",
        "description": "Temperature within ±1°C of 25°C",
        "input": "25 50 1013.25 0 0\n",
        "checks": [
            {"type": "range", "field": "Temperature", "min": 24.0, "max": 26.0},
            {"type": "time_within", "tolerance": 3}
        ]
    },
    {
        "id": "TC002",
        "description": "Humidity within ±0.05 of 0.50",
        "input": "25 50 1013.25 0 0\n",
        "checks": [
            {"type": "range", "field": "Humidity", "min": 45.0, "max": 55.0},
            {"type": "time_within", "tolerance": 3}
        ]
    },
    {
        "id": "TC003",
        "description": r"Pressure within ±1 hPa of 1013.25",
        "input": "25 50 1013.25 0 0\n",
        "checks": [
            {"type": "range", "field": "Pressure", "min": 1012.25, "max": 1014.25},
            {"type": "time_within", "tolerance": 3}
        ]
    },
    {
        "id": "TC004",
        "description": r"Validate real-time UI data refresh",
        "input": "25 50 1013.25 0 0\n27 52 1011.5 0 0\n",
        "checks": [
            {"type": "range", "field": "Temperature", "min": 26.0, "max": 28.0},
            {"type": "range", "field": "Humidity", "min": 47.0, "max": 57.0},
            {"type": "range", "field": "Pressure", "min": 1010.5, "max": 1012.5},
            {"type": "time_within", "tolerance": 3}
        ]
    },
    {
        "id": "TC005",
        "description": "Historical data stored in storage.txt",
        "input": "25 50 1013.25 0 0\n",
        "checks": [
            {"type": "file_exists", "file": "storage.txt"},
            {"type": "time_within", "tolerance": 3}
        ]
    }
]
