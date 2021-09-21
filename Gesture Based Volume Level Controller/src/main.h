/// @file main.h
/// @author Geike Hannes Mathias (IFAG PSS RFS 3DS RS PM) <HannesMathias.Geike@infineon.com>
/// @date 11-02-2020
/// @version V0.0

#ifndef MAIN_H
#define MAIN_H

/***************** ENUMS AND STRUCT DEFINITIONS ********************/

/// Control info property struct to determine and calculate control
/// behaviour during the control state.
typedef struct CtrlInfo{
  const float levelPTime;     // Level gradient over time (in % per seconds)
  const float updateRate;     // Time interval between level adjustments (in Hz)
  const unsigned long gatingTime; // Time until exit motion is detectable (in ms)
  unsigned long startTime;    // Timestamp control mode was entered (in ms)
  unsigned long lastUpdated;
  unsigned long counter;
}CtrlInfo;

/// Predefined RGB color for the onboard RGB LED on the MKR
typedef enum RgbColor   // RGB LED color scheme
{  
  NONE,
  RED,
  GREEN,
  BLUE,
  WHITE
}RgbColor;




/// Control FSM states
typedef enum CtrlState
{
  STANDBY,
  GESTURE,
  COMMAND
}CtrlState; 

/// Gestrue detection FSM states
typedef enum GstrState
{
  PATTERN0,
  PATTERN1,
  PATTERN2,
  PATTERN3,
  PATTERN4
}GstrState;

/************************* My Functions ****************************/

/// Inizializes the onboard RGB LED on the MKR 1010.
void initRGBLed(void);

/// Sets the onboard RGB LED color and brightnes.
/// @param color Predefined RGB color
/// @param level Brighntesslevel
void setRGBLed(RgbColor color, unsigned char level);

/// Detects waving-2-times gesture.
/// @param gstrstate Pointer to current gesture state enumerator buffer
/// @param gstrtime Timestamp pointer for gesture detection
/// @return : true, if gesture was successfully detected
///         : false, if gesture is not completed yet
bool wavingGesture2(GstrState* gstrstate, unsigned long* gstrtime);

/// Detects waving-2.5-times gesture.
/// @param gstrstate Pointer to current gesture state enumerator buffer
/// @param gstrtime Timestamp pointer for gesture detection
/// @return : true, if gesture was successfully detected
///         : false, if gesture is not completed yet
bool wavingGesture3(GstrState* gstrstate, unsigned long* gstrtime);

/// Converts the gesture in front of the sensor shield into a increasing
/// or decreasing output level.
/// @param outlevel Pointer to output level value to be in/decreased
/// @param info Auxiliary controll struct containing app-specific control properties
/// @return : true, if exit gesture has been successfully detected
///         : false, if still in control mode
bool linearControl(float* outlevel, CtrlInfo* info);

/// Initializes a simple, dimmable LED on the handled PWM output pin. 
/// This function can also be used to controll other devices via signal output.
/// @param pwmpin Number of PWM pin LED is connected to (via resistor).
void initSimpleLED(unsigned char pwmpin);

/// Sets the simple LEDs PWM output linearily in accordance to the 
/// handled output level.
/// @param outlevel Output level to be set
void setSimpleLED(float outlevel);

/// Sets the PC system volume in accordance to the the handled output
/// level by sending it via serial connection. 
/// To do so, start script 'volume_control.py' in the test folder
/// of this project.
/// handled output level.
/// @param outlevel Output level to be set
void setPCVolume(float outlevel);


#endif /* MAIN_H */