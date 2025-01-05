#include "interface.h"
#include "core/powerSave.h"
#include <AXP192.h>
AXP192 axp192;

// Menu state
int currentOption = 0;
const int totalOptions = 3;  // Total number of menu options

// Tap detection variables
float accX, accY, accZ;  // Accelerometer readings
float tapThreshold = 1.5;  // Threshold to detect a tap (adjust as needed)

// Menu control
bool menuActive = true;  // Determines if we're in the menu or running an option

// IMU data variables
float gyroX = 0.0F, gyroY = 0.0F, gyroZ = 0.0F;
float pitch = 0.0F, roll = 0.0F, yaw = 0.0F;
float temp = 0.0F;

// Timing variables for debouncing
unsigned long lastTapTime = 0;
unsigned long debounceDelay = 300;  // 300 ms debounce time

/***************************************************************************************
** Function name: _setup_gpio()
** Description:   Initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    pinMode(SEL_BTN, INPUT);
    pinMode(DW_BTN, INPUT);
    pinMode(UP_BTN, INPUT);
    pinMode(BACK_BTN, INPUT);  // Assuming BACK_BTN is Button B for back functionality
    axp192.begin();           // Start the energy management of AXP192
}

/***************************************************************************************
** Function name: getBattery()
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
    int percent = 0;
    float b = axp192.GetBatVoltage();
    percent = ((b - 3.0) / 1.2) * 100;
    return (percent < 0) ? 0 : (percent >= 100) ? 100 : percent;
}

/*********************************************************************
**  Function: setBrightness
**  Set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    if (brightval > 100) brightval = 100;
    axp192.ScreenBreath(brightval);
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    // Detect tap input
    M5.Imu.getAccelData(&accX, &accY, &accZ);
    unsigned long currentMillis = millis();
    if ((fabs(accX) > tapThreshold || fabs(accY) > tapThreshold || fabs(accZ) > tapThreshold) && 
        (currentMillis - lastTapTime > debounceDelay)) {
        scrollMenu();  // Call the scroll function to move through the menu
        lastTapTime = currentMillis;
    }

    // Handle physical button presses
    if (digitalRead(UP_BTN) == LOW || digitalRead(SEL_BTN) == LOW || digitalRead(DW_BTN) == LOW) {
        if (!wakeUpScreen()) AnyKeyPress = true;
        else goto END;
    }
    if (digitalRead(UP_BTN) == LOW) {
        PrevPress = true;
        EscPress = true;
    }
    if (digitalRead(DW_BTN) == LOW) {
        NextPress = true;
    }
    if (digitalRead(SEL_BTN) == LOW) {
        SelPress = true;
    }
    END:
    if (AnyKeyPress) {
        long tmp = millis();
        while ((millis() - tmp) < 200 && (digitalRead(UP_BTN) == LOW || digitalRead(SEL_BTN) == LOW || digitalRead(DW_BTN) == LOW));
    }
}

/***************************************************************************************
** Function: scrollMenu
** Description:   Scroll through the menu options
***************************************************************************************/
void scrollMenu() {
    currentOption++;
    if (currentOption >= totalOptions) {
        currentOption = 0;  // Loop back to the top
    }
    updateMenuDisplay();
}

/***************************************************************************************
** Function: selectOption
** Description:   Select the current menu option
***************************************************************************************/
void selectOption() {
    // Handle the logic when the current option is selected
    if (currentOption == 0) {
        // Execute option 0 functionality
    } else if (currentOption == 1) {
        // Execute option 1 functionality
    } else if (currentOption == 2) {
        // Execute option 2 functionality
    }
}

/***************************************************************************************
** Function: goBack
** Description:   Go back to the previous menu or screen
***************************************************************************************/
void goBack() {
    menuActive = false;  // Example: Deactivate the menu to exit or go back to the main menu
    updateMenuDisplay();  // Optionally update the display to reflect the change
}

/***************************************************************************************
** Function: updateMenuDisplay
** Description:   Update the screen to display the current menu option
***************************************************************************************/
void updateMenuDisplay() {
    M5.Lcd.clear();  // Clear the screen
    // Display the menu options and highlight the current selection
    for (int i = 0; i < totalOptions; i++) {
        if (i == currentOption) {
            M5.Lcd.setTextColor(RED);  // Highlight current option in red
        } else {
            M5.Lcd.setTextColor(WHITE);  // Other options in white
        }
        M5.Lcd.setCursor(20, 30 + i * 40);
        M5.Lcd.print("Option ");
        M5.Lcd.print(i + 1);
    }
}

/***************************************************************************************
** Function: powerOff
** Description:   Power off the device
***************************************************************************************/
void powerOff() {
    axp192.PowerOff();
}

/***************************************************************************************
** Function: checkReboot
** Description:   Handle the long press for reboot or power off
***************************************************************************************/
void checkReboot() {
    int countDown;
    /* Long press power off */
    if (axp192.GetBtnPress()) {
        uint32_t time_count = millis();
        while (axp192.GetBtnPress()) {
            // Display poweroff bar only if holding button
            if (millis() - time_count > 500) {
                tft.setCursor(60, 12);
                tft.setTextSize(1);
                tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                countDown = (millis() - time_count) / 1000 + 1;
                tft.printf(" PWR OFF IN %d/3\n", countDown);
                delay(10);
            }
        }
        // Clear text after releasing the button
        delay(30);
        tft.fillRect(60, 12, tftWidth - 60, tft.fontHeight(1), bruceConfig.bgColor);
    }
}
