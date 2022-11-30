# Platformio VS Code Extension

With the chosen MCU, the most natural DE would be the proprietary STM32CubeIDE, however, through testing with the E-Ink display, we found that the overall structure and the use of the Eclipse platform was halting our progress.

With this, we searched for an alternative to the proprietary IDE and found a VS Code extension, `Platformio`, which allows for compilation of Arduino formatted code which is then converted to and compiled as STM's HAL functions and implementations. This means, that we can use Arduino libraries and code formatting (which is basically `C`) which simplifies the whole process.

Now that a platform for the project has been decided, 