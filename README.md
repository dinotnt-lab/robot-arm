# Robot Arm

Idk, I just wanted a cool robot arm that can move by itself and pick things up.

## Version 1

### Overview

<img src="https://raw.githubusercontent.com/dinotnt-lab/robot-arm/refs/heads/main/images/PXL_20261001_173940025.jpg" width="300">

3-arm robot arm (2 in image; missing is one with the grabber)

4 × 28BYJ-48 stepper motors

* Base motor for spin (idk the terms, like pitch or something)
* Elbow motors for reach + height

ESP32

3D-printed arms

UI for individual motor control hosted on the ESP32 with Wi-Fi

### Problems

* Motors have too large a margin of error

  * They wiggle a little before the motor mechanism catches it
  * Fix: better motors
* Motors are not strong enough

  * Fix: more power **FAILED**
  * Fix 2: new motors

**Conclusion for V1:** New motors needed.
