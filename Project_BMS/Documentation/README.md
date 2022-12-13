# Platformio VS Code Extension

With the chosen MCU, the most natural DE would be the proprietary STM32CubeIDE, however, through testing with the E-Ink display, we found that the overall structure and the use of the Eclipse platform was halting our progress.

With this, we searched for an alternative to the proprietary IDE and found a VS Code extension, `Platformio`, which allows for compilation of Arduino formatted code which is then converted to and compiled as STM's HAL functions and implementations. This means, that we can use Arduino libraries and code formatting (which is basically `C`) which simplifies the whole process.

Now that a platform for the project has been decided, some of the software responsibilities needs to be split up.
Implementation of the E-Ink UI software has been done by using the baseline software demo provided by Waveshare and forming it to fit the projects needs.

Through testing and speaking with the `PO`, [the following design has been chosen for the UI](https://github.com/Olliyard/Upteko/blob/master/Project_BMS/E-Ink_UI.svg). With the design 
