// Written for C language (may be subject to change)
// STM32L475RGT6

// Initialize device components
// 	Disable interrupts
// 	Init timer for ADC
// 	Init screen (LED/OLED)
// 	Init alarm
// 	Init device I/O (switches, buttons, touchscreen??)
// 	Init timers for sensor frequency
// Prompt user for upper and lower glucose ranges
// Enable interrupts
// While(TRUE)
// 	Wait for timer
// 		Print glucose measurement
// 		Calculate trend in changes
// 		Alert if out of/approaching limit
// 		Save data to log
// End while
// Save measurement output file
