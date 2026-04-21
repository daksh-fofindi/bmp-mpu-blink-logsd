Author - Daksh Fofindi (Daksh Aerospace)

# If you liked my work please follow me on below social media profiles

youtube - https://m.youtube.com/@dakshfofindi907/videos?view=0&sort=dd&shelf_id=2
<br>
Website - http://www.dakshaerospace.in/
<br>
instagram - https://www.instagram.com/dakshfofindi/
<br>
X - https://x.com/dakshfofindi
<br>
LinkedIn - https://in.linkedin.com/in/dakshfofindi



# Flight Data Logger System

A high-frequency embedded data logging system designed for rocket avionics.  
This system collects real-time sensor data and stores it on an SD card for post-flight analysis.


## Overview

This project is a **flight computer data logger** built using ESP32.  
It integrates multiple sensors to record critical flight parameters such as:

- Atmospheric data (Temperature, Pressure, Altitude)
- Motion data (Acceleration & Gyroscope)
- Timestamped logging at high frequency (10Hz)

Designed for **rocket launches, testing, and avionics development**.


##  Hardware Used

- ESP32 Microcontroller  
- BMP280 (Pressure + Temperature Sensor)  
- MPU6050 (Accelerometer + Gyroscope)  
- SD Card Module  
- Buzzer & Status LED  



##  Features

-  **High-speed logging** (every 100ms)
-  **Multi-sensor integration**
-  **CSV data storage** for easy analysis
-  **Non-blocking architecture** using millis()
-  **Status indication** via LED & buzzer
-  Ready for **rocket flight data analysis**



##  Data Format

Logged file: `FLIGHT_LOG.csv`

```csv
Time_ms,Temp_C,Pressure_hPa,Altitude_m,
AccX_g,AccY_g,AccZ_g,
GyroX_dps,GyroY_dps,GyroZ_dps
