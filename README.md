# Smart Medibox

The **Smart Medibox** is an intelligent system designed to enhance the storage and management of medicines. It monitors light intensity inside the box and automatically adjusts a sliding window to protect light-sensitive medications. The system is controlled via a user-friendly Node-RED dashboard that allows both manual and automatic control options.

![Smart Medibox](path-to-image) <!-- Replace this with the actual image path -->

## Key Features

- **Light Intensity Monitoring**: Continuously monitors light conditions inside the Medibox to protect medications from excessive light exposure.
- **Servo-Controlled Sliding Window**: Automatically adjusts the window to regulate light exposure based on the monitored conditions.
- **Node-RED Dashboard**: Provides real-time monitoring, manual control, and threshold configuration for automating the sliding window.
- **Automation**: Ensures light-sensitive medications are always stored in optimal conditions.

## System Overview

### Hardware Components:
- **Light Sensor**: Measures the light intensity inside the Medibox.
- **Servo Motor**: Controls the sliding window based on light sensor data.
- **Microcontroller (Arduino/ESP32)**: Processes sensor data and controls the servo motor.

### Software Components:
- **Node-RED Dashboard**: A web-based interface to control and monitor the Medibox. It allows users to:
  - Monitor the real-time light intensity.
  - Set desired light threshold levels.
  - Manually control the sliding window.
- **Arduino Code**: Connects the microcontroller to the light sensor and servo motor, enabling the automatic adjustment of the window.


## Getting Started

Follow these instructions to set up and run the Smart Medibox system.

### Prerequisites

- **Hardware**:
  - Light sensor (LDR or any suitable sensor).
  - Servo motor.
  - Microcontroller (Arduino, ESP32).
  - Jumper wires and other necessary connections.

- **Software**:
  - [Node-RED](https://nodered.org) installed on your computer or Raspberry Pi.
  - [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/) for uploading code to the microcontroller.

## Setup

### Hardware Setup:

1. **Connect the Light Sensor and Servo Motor**:
   - Connect the light sensor and servo motor to the microcontroller as shown in the circuit diagram (available in the `images` folder).
   
2. **Upload the Code**:
   - Upload the code in the `arduino-code` directory to your microcontroller using Arduino IDE or PlatformIO.

### Node-RED Setup:

1. **Import the Node-RED Flow**:
   - Import the provided Node-RED flow from the `node-red-flows` directory into your Node-RED instance.
   
2. **Start the Node-RED Service**:
   - Start your Node-RED service and navigate to the dashboard to control and monitor the Medibox.

### Run the System:

1. **Monitor the System**:
   - Monitor the light intensity in real-time via the dashboard and let the system automatically control the sliding window.

## Node-RED Dashboard

The Node-RED dashboard offers an intuitive interface to interact with the Medibox.

### Dashboard Features:
- **Real-Time Monitoring**: Displays the current light intensity inside the box.
- **Threshold Settings**: Allows users to configure light thresholds for automatic window control.
- **Manual Control**: Users can manually open or close the sliding window.

![Dashboard Screenshot](path-to-dashboard-image) <!-- Replace this with the actual dashboard image path -->



