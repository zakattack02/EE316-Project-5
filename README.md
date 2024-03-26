
# EE 316 Computer Engineering Junior Lab - Design Project 5 - Spring 2024

Welcome to the repository for Design Project 5 of the EE 316 Computer Engineering Junior Lab for Spring 2024. This project involves the development of a digital system using the Xilinx ZynQ processor. Below you will find all the necessary information, specifications, parts list, and guidelines for the project.

## Specification
- **Digital System**: A digital system using Xilinx ZynQ processor.
- **Due Date**: Thursday, April 9 (Project Submission), Friday, April 11 (Written Report Submission).
- **Parts List**:
  1. Digilent Cora-Z7-07S
  2. PMOD buttons and LEDs
  3. Arduino Starter Kit components:
     - Photoresistor(s)
     - Potentiometer
     - Ultrasonic sensor (HC-SC04)
     - Servo Motor (SG90)
     - DC motor
     - Passive buzzer
     - LCD module (I2C)
     - 3.3/5V Power Supply

## Project Description
Develop a digital system capable of sampling 8-bit data from any available analog inputs on Digilent’s Cora Z7 on the Analog header. The system will then control the angular position of a servo motor, the intensity of PMOD LEDs, and the speed of a DC motor.

## Design Specifications
- Utilize Digilentinc's Cora-Z7 board, Xilinx's Vivado, and SDK tools for design.
- Use Xilinx's Zynq processor and other IPs for system design, pin assignment, bitstream generation, and export to Xilinx SDK.
- Control servo motor and PMOD LEDs with potentiometer.
- Control servo motor rotation (-90 to +90 degrees) using PWM signal.
- Control PMOD LEDs' intensity using AXI Timer's PWM output.
- Use light-sensitive resistor to control DC motor and buzzer.
- Use onboard buttons with interrupts for system control.
- Display system status and analog source on LCD.
- Utilize ultrasonic sensor for object detection and alerts.
- Implement colored LED and sound alarm based on object distance.
- Debug design using logic analyzer.
- Conduct data transactions on selected bus for functionality demonstration.

## Useful Links
- [Microzed Chronicles](https://www.adiuvoengineering.com/microzed-chronicles-archive)
- [Zynq TRM](https://www.xilinx.com/support/documentation/user_guides/ug585-Zynq-7000-TRM.pdf)
- [Zynq Architecture](https://www.xilinx.com/products/silicon-devices/soc/zynq-7000.html)
- [AXI XADC](https://www.xilinx.com/support/documentation/ip_documentation/axi_xadc/v5_1/pg091-axi-xadc.pdf)
- [AXI Timer](https://www.xilinx.com/support/documentation/ip_documentation/axi_timer/v2_0/pg079-axi-timer.pdf)
- [HC-SR04 Ultrasonic Sensor Datasheet](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf)
- [Complete Guide for HC-SR04 Ultrasonic Sensor](https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/)

## Teams Members
- Zak Konik
- Nick Engle
- Olaoluwayimika olugbenle

---

Feel free to modify this README file according to your specific project needs. Good luck with your project! If you have any questions or need further assistance, don't hesitate to ask.
