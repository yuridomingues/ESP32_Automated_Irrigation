# ESP32 Automated Irrigation

An efficient and automated irrigation system leveraging ESP32 and Arduino. This project is designed to monitor and maintain soil moisture levels for optimal plant growth. By integrating sensors, a relay module, and a pump, the system can autonomously water plants based on real-time data.

---

## Features
- **Soil Moisture Monitoring**: Uses sensors to continuously monitor soil humidity.
- **Automated Watering**: Activates a water pump when moisture levels fall below the configured threshold.
- **Customizable Settings**: Allows calibration of sensor thresholds for different types of soil and plants.
- **ESP32 Integration**: Facilitates remote monitoring and potential IoT functionalities.

---

## Getting Started
### Hardware Requirements
- ESP32 microcontroller
- Soil moisture sensor
- Water pump
- Relay module
- Power supply (as per pump specifications)
- Jumper wires and breadboard

### Software Requirements
- Arduino IDE (with ESP32 board manager installed)
- Libraries: [DHT Sensor Library](https://github.com/adafruit/DHT-sensor-library), [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor)

---

## Installation
1. Clone this repository:
   ```bash
   git clone https://github.com/yuridomingues/ESP32_Automated_Irrigation.git
   ```

2. Open the `.ino` file in Arduino IDE.

3. Configure your board and port in the IDE:
   - Board: `ESP32 Dev Module`
   - Port: Select your device's COM port

4. Upload the code to your ESP32 after connecting it to your computer.

---

## Usage
1. Connect the hardware components as per the circuit diagram provided in the repository.
2. Power up the ESP32 and monitor the serial output for system status.
3. Adjust sensor thresholds in the code if necessary.

---

## Contributing
Contributions are welcome! To contribute:
1. Fork the repository.
2. Create a new branch for your feature or bugfix.
3. Commit your changes and push them to your fork.
4. Open a pull request with a detailed description of your changes.

---

## Acknowledgments
A special thank you to the people I did this project with

- [**Sneeker**](https://github.com/TechSneeker).
- [**Ricardo Andrade**](https://github.com/ric-cfan).

---

## License
This project is licensed under the [MIT License](LICENSE).
