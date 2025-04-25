IoT Security System with Arduino  
Components Used: Arduino Uno, Ultrasonic Sensor, Buzzer, IR Remote, IR Receiver, Breadboard, Servo Motor, Wires, LCD Display  

## 📌 Overview  
A smart security system integrating IoT components to detect intrusions, manage access via IR remote, and provide real-time feedback through an LCD and buzzer.  

## 🛠 Features  
- Motion Detection: Ultrasonic sensor triggers alerts for objects within 10 cm.  
- IR Remote Control: Lock/unlock system or set passwords using a remote.  
- LCD Feedback: Displays status messages (e.g., "Access Denied", "Motion Detected").  
- Buzzer Alerts: Audible alarms during intrusions or invalid access attempts.  
- Servo Motor Lock: Physical lock mechanism (90° = LOCKED, 0° = UNLOCKED).  
- Breadboard Setup: Organized wiring for easy replication.  

## 🔌 Hardware Setup  
| Component          | Quantity | Purpose                                |  
|---------------------|----------|----------------------------------------|  
| Arduino Uno         | 1        | Microcontroller brain                 |  
| Ultrasonic Sensor   | 1        | Detects nearby objects                |  
| IR Remote & Receiver| 1        | Wireless system control               |  
| Servo Motor         | 1        | Physical lock mechanism               |  
| Buzzer              | 1        | Audible alerts                        |  
| 16x2 LCD Display    | 1        | User interface for status messages    |  
| Breadboard          | 1        | Secure component connections          |  
| Jumper Wires        | 15+      | Connect components to Arduino         |  

## ⚙️ Installation & Wiring  
1. Clone the Repository
2. Breadboard Connections: Follow the Wiring Guide for detailed pin mappings.
3. Libraries: Install via Arduino IDE: LiquidCrystal (for LCD), IRremote (for IR receiver), Servo (for servo motor)
4. Upload Code: Open AlarmSystem.ino and upload to Arduino.

🎥 Demonstration
Demo Video: Watch the system in action here. Shows IR remote interaction, LCD feedback, buzzer alerts, and servo locking.
Iteration Log: Track design changes based on user feedback.

📈 Future Improvements
Wi-Fi Integration: Send alerts to smartphones via ESP8266.
Encrypted IR Signals: Prevent spoofing attacks.
Enhanced UI: Add buttons for manual override on the breadboard.

# Hardware Wiring Guide  

## Breadboard Connections  
| Component          | Arduino Pin | Connection Details               |  
|---------------------|-------------|-----------------------------------|  
| Ultrasonic Trig     | 4           | Digital output                   |  
| Ultrasonic Echo     | 6           | Digital input                    |  
| Buzzer              | 3           | Digital output (PWM)             |  
| IR Receiver         | 13          | Digital input                    |  
| Servo Motor         | 5           | PWM control                      |  
| LCD RS              | 7           | Register Select                  |  
| LCD Enable          | 8           | Enable pin                       |  
| LCD D4-D7           | 9-12        | Data pins                        |  
  
