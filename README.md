# Smart Medibox

The Smart Medibox is an intelligent system designed to protect temperature-sensitive medications by automatically adjusting the exposure to light. It monitors the light intensity inside the box and controls a sliding window using a servo motor, which adjusts based on predefined light thresholds.

## Project Features

- Real-time light intensity monitoring
- Automated sliding window control based on light conditions
- User-configurable light thresholds
- Manual control option via Node-RED dashboard

## Demo

You can view the project simulation on Wokwi: [Wokwi Simulation](https://wokwi.com/projects/your-simulation-link)

## Node-RED Dashboard

The Node-RED dashboard offers an intuitive interface to interact with the Medibox.

**Dashboard Features:**

- **Real-Time Monitoring**: Displays the current light intensity inside the box.
- **Threshold Settings**: Allows users to configure light thresholds for automatic window control.
- **Manual Control**: Users can manually open or close the sliding window.

<!-- Add Node-RED dashboard screenshot here -->
![Node-RED Dashboard](path-to-your-dashboard-image)

## System Components

1. **Light Sensor**: Measures light intensity inside the Medibox.
2. **Servo Motor**: Controls the sliding window to regulate light exposure.
3. **Node-RED**: Provides a user interface for real-time monitoring and control.
4. **Wokwi Simulation**: The circuit is designed and simulated using the Wokwi platform.

## How to Run the Project

### 1. Wokwi Circuit Setup

The circuit is built and simulated on the Wokwi platform. You can access it using the following link:

[Wokwi Simulation](https://wokwi.com/projects/your-simulation-link)

### 2. Node-RED Setup

1. Import the provided Node-RED flow from the `node-red-flows` directory into your Node-RED instance.
2. Start your Node-RED service and navigate to the dashboard to control and monitor the Medibox.

### 3. Run the System

- Monitor the light intensity in real-time via the Node-RED dashboard.
- The system will automatically control the sliding window based on light thresholds.
- Optionally, manually open or close the sliding window from the dashboard.

## Future Enhancements

- **Temperature Sensor Integration**: Add temperature monitoring to further protect medications.
- **Mobile App Interface**: Develop a mobile app for remote control and monitoring.
- **Alert Notifications**: Implement notifications via email or SMS for threshold breaches.

## Screenshots

<!-- Add relevant screenshots of the Wokwi circuit and Node-RED dashboard -->
![Wokwi Circuit](path-to-your-wokwi-circuit-image)
![Node-RED Flow](path-to-your-node-red-flow-image)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
