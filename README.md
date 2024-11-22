# VOXY: Voice Recognition Project

## Overview

This project implements a voice recognition system on an embedded platform, leveraging Mel filter coefficients and machine learning models to classify audio signals.

## Hardware Used

The project was developed using the **B-L475E-IOT01A** development board, which provides built-in support for audio capture and other IoT functionalities.

## Software References

The audio capture functionality is based on the program available at the following repository:  
[https://github.com/RicardoCLake/b-l475e-iot01a-audio-mbed](https://github.com/RicardoCLake/b-l475e-iot01a-audio-mbed).

This repository provided the foundation for handling sound input from the microphone on the B-L475E-IOT01A board.

## Project Structure

### 1. **`reconaissance vocal`**

This folder contains the core **Mbed** project files. It includes the implementation of the voice recognition system, with a particular emphasis on the **`sources`** folder.  

The **`sources`** folder contains:
- Code to preprocess audio signals, including Mel filter application.
- Implementation of the machine learning model for classifying audio commands.

### 2. **Auxiliary Python Scripts**

This folder contains Python scripts that were used to:
- Calculate Mel filter coefficients.
- Train and export machine learning models that are later deployed on the embedded system.

These scripts complement the embedded code by preparing essential data and models during the development process.

## How to Use

1. Clone the repository and navigate to the **`reconaissance vocal`** folder.
2. Build and flash the project to the B-L475E-IOT01A board.
3. Optionally, use the scripts in **`Auxiliary Python Scripts`** to preprocess new data or train updated models.
