#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <GamecubeController.hpp>
#include <TUGamepad.hpp>
#include <gamecube_definitions.h>

#include "GamecubeConsole.hpp"
#include "gamecube_definitions.h"
#include <Arduino.h>

#include <input/gpio.hpp>
#include <Adafruit_NeoPixel.h>

#include <Adafruit_USBD_XInput.hpp>
#include <pico/bootrom.h>
#include "TUCompositeHID/TUKeyboard.hpp"

hid_gamepad_hat_t GetDpadAngle(bool left, bool right, bool down, bool up);

TUGamepad *gamepad;
GamecubeController *gcc;
GamecubeConsole *gc;
volatile gc_report_t gc_report = default_gc_report;
int mode = 0;
int poll = 0;

Adafruit_NeoPixel pixels(1, 25, NEO_GRB + NEO_KHZ800);

Adafruit_USBD_XInput *_xinput;
xinput_report_t _report = {};

TUKeyboard *keyboard;

#define GC_CONSOLE_PIN 2
#define GCC_PIN 21
#define LZ_PIN 6

#define sticks_deadzone 8
#define triggers_deadzone 40

#define sticks_sensitivity 64 //Used for Keyboard mode. The lower, the more sensible the sticks are.

void setup() {
    pixels.begin();
    pixels.setPixelColor(0, pixels.Color(5, 0, 0)); // Red light indicates that the firmware is booting
    pixels.show();

    gpio::init_pin(LZ_PIN, gpio::GpioMode::GPIO_INPUT_PULLUP); //L1 (Z2). Only does something if you are adding a second z button.

    //GAMECUBE SETUP
    if (GamecubeConsole(GC_CONSOLE_PIN).Detect()) {
       gc = new GamecubeConsole(GC_CONSOLE_PIN, pio0);
       mode=1;
    }

    while (poll==0)
        delay(1);

    if (gc_report.start)
        reset_usb_boot(0, 0);
    
    //XINPUT SETUP
    if (gc_report.x) {    
        mode=2;
        Serial.end();
        _xinput = new Adafruit_USBD_XInput();
        _xinput->begin();
        Serial.begin(115200);

        USBDevice.setManufacturerDescriptor("Nintendo");
        USBDevice.setProductDescriptor("Gamecube Controller");
        USBDevice.setID(0x057e, 0x1111);

        while (!_xinput->ready()) {
            delay(1);
        }
    }

    //KEYBOARD SETUP
    else if (gc_report.z) {    
        mode=3;
        keyboard = new TUKeyboard();
        USBDevice.setManufacturerDescriptor("Nintendo");
        USBDevice.setProductDescriptor("Gamecube Controller");
        USBDevice.setID(0x057e, 0x1111);
        TUKeyboard::registerDescriptor();
        keyboard->begin();
    }

    //DINPUT SETUP
    else {
        gamepad = new TUGamepad();
        USBDevice.setManufacturerDescriptor("Nintendo");
        USBDevice.setProductDescriptor("Gamecube Controller");
        USBDevice.setID(0x057e, 0x1111);
        gamepad->begin();
        }

    while (!USBDevice.ready()&&mode!=1) {
        delay(1);
    }
}

void loop() {

    //DINPUT
    if (mode==0) {
        pixels.setPixelColor(0, pixels.Color(5, 5, 0)); // Yellow light means that the firmware booted in DInput Mode
        pixels.show();

        // Digital outputs
        gamepad->setButton(0, gc_report.a);  // A-X
        gamepad->setButton(1, gc_report.b);  // B-O
        gamepad->setButton(2, gc_report.x);  // X-□
        gamepad->setButton(3, gc_report.y);  // Y-△
        gamepad->setButton(4, !gpio::read_digital(LZ_PIN));  // L1
        gamepad->setButton(5, gc_report.z);  // R1
        gamepad->setButton(6, gc_report.l);  // L2 DIGITAL
        gamepad->setButton(7, gc_report.r);  // R2 DIGITAL
        gamepad->setButton(9, gc_report.start);  // START-OPTIONS


        // Analog outputs
        if (gc_report.stick_x>128+sticks_deadzone||gc_report.stick_x<128-sticks_deadzone)
            gamepad->leftXAxis(gc_report.stick_x + 128);  // LEFT STICK X
        else
            gamepad->leftXAxis(0);
        if (gc_report.stick_y>128+sticks_deadzone||gc_report.stick_y<128-sticks_deadzone)
            gamepad->leftYAxis( - gc_report.stick_y + 128);  // LEFT STICK Y
        else
            gamepad->leftYAxis(0);
        if (gc_report.cstick_x>128+sticks_deadzone||gc_report.cstick_x<128-sticks_deadzone)
            gamepad->triggerLAnalog(gc_report.cstick_x + 128);  // RIGHT STICK X
        else
            gamepad->triggerLAnalog(0);
        if (gc_report.cstick_y>128+sticks_deadzone||gc_report.cstick_y<128-sticks_deadzone)
            gamepad->rightXAxis( - gc_report.cstick_y + 128);  // RIGHT STICK Y
        else
            gamepad->rightXAxis(0);
        if (gc_report.l_analog>triggers_deadzone)
            gamepad->triggerRAnalog(gc_report.l_analog + 129);  // L2 TRIGGER
        else
            gamepad->triggerRAnalog(129);
        if (gc_report.r_analog>triggers_deadzone)
            gamepad->rightYAxis(gc_report.r_analog + 129);  // R2 TRIGGER
        else
            gamepad->rightYAxis(129);
        // D-pad Hat Switch
        gamepad->hatSwitch(
            gc_report.dpad_left,
            gc_report.dpad_right,
            gc_report.dpad_down,
            gc_report.dpad_up
        );
        
        gamepad->sendState();
    }

    //GAMECUBE
    if (mode==1) {      
        pixels.setPixelColor(0, pixels.Color(3, 0, 15)); // Purple light indicates that the firmware booted in Gamecube mode
        pixels.show();
        
        gc->WaitForPoll();
        gc_report.z = !gpio::read_digital(LZ_PIN)+gc_report.z; // The additional button defaults to a secondary z button
        gc->SendReport((gc_report_t *)&gc_report);
    }

    //XINPUT
    if (mode==2) {
        pixels.setPixelColor(0, pixels.Color(0, 5, 0)); // Green light means that the firmware booted in XInput Mode
        pixels.show();

        // Digital outputs
        _report.a = gc_report.a;
        _report.b = gc_report.b;
        _report.x = gc_report.x;
        _report.y = gc_report.y;
        _report.rb = gc_report.z;
        _report.lb = !gpio::read_digital(LZ_PIN);
        _report.start = gc_report.start;
        _report.dpad_up = gc_report.dpad_up;
        _report.dpad_down = gc_report.dpad_down;
        _report.dpad_left = gc_report.dpad_left;
        _report.dpad_right = gc_report.dpad_right;
        // Analog outputs
        if (gc_report.stick_x>128+sticks_deadzone||gc_report.stick_x<128-sticks_deadzone)
            _report.lx = (gc_report.stick_x - 128) * 65535 / 255 + 128;  // LEFT STICK X
        else
            _report.lx = 0;
        if (gc_report.stick_y>128+sticks_deadzone||gc_report.stick_y<128-sticks_deadzone)
            _report.ly = (gc_report.stick_y - 128) * 65535 / 255 + 128;  // LEFT STICK Y
        else
            _report.ly = 0;
        if (gc_report.cstick_x>128+sticks_deadzone||gc_report.cstick_x<128-sticks_deadzone)
            _report.rx = (gc_report.cstick_x - 128) * 65535 / 255 + 128;  // RIGHT STICK X
        else
            _report.rx = 0;
        if (gc_report.cstick_y>128+sticks_deadzone||gc_report.cstick_y<128-sticks_deadzone)
             _report.ry = (gc_report.cstick_y - 128) * 65535 / 255 + 128;  // RIGHT STICK Y
        else
            _report.ry = 0;
        if (gc_report.l_analog>triggers_deadzone)
            _report.lt = gc_report.l_analog;  // L2 TRIGGER
        else
            _report.lt = 0;
        if (gc_report.r_analog>triggers_deadzone)
            _report.rt = gc_report.r_analog;  // R2 TRIGGER
        else
            _report.rt = 0;

        while (!_xinput->ready()) {
            tight_loop_contents();
        }

        _xinput->sendReport(&_report);
    }

    //KEYBOARD
    if (mode==3) {
        pixels.setPixelColor(0, pixels.Color(0, 0, 10)); // Blue light means that the firmware booted in Keyboard Mode
        pixels.show();
        
        // Digital outputs
        keyboard->setPressed(HID_KEY_P, gc_report.a);  // A-X
        keyboard->setPressed(HID_KEY_O, gc_report.b);  // B-O
        keyboard->setPressed(HID_KEY_I, gc_report.x);  // X-□
        keyboard->setPressed(HID_KEY_U, gc_report.y);  // Y-△
        keyboard->setPressed(HID_KEY_Z, !gpio::read_digital(LZ_PIN));  // L1
        keyboard->setPressed(HID_KEY_X, gc_report.z);  // R1
        keyboard->setPressed(HID_KEY_C, gc_report.l); // L2 DIGITAL
        keyboard->setPressed(HID_KEY_V, gc_report.r);  // R2 DIGITAL
        keyboard->setPressed(HID_KEY_ENTER, gc_report.start);  // START-OPTIONS

        keyboard->setPressed(HID_KEY_ARROW_UP, gc_report.dpad_up);
        keyboard->setPressed(HID_KEY_ARROW_DOWN, gc_report.dpad_down);
        keyboard->setPressed(HID_KEY_ARROW_LEFT, gc_report.dpad_left);
        keyboard->setPressed(HID_KEY_ARROW_RIGHT, gc_report.dpad_right);

        // Analog outputs
        if (gc_report.stick_x>128+sticks_sensitivity)
            keyboard->setPressed(HID_KEY_D, 1);  // LEFT STICK LEFT
        else
            keyboard->setPressed(HID_KEY_D, 0);
        if (gc_report.stick_x<128-sticks_sensitivity)
            keyboard->setPressed(HID_KEY_A, 1);  // LEFT STICK RIGHT
        else
            keyboard->setPressed(HID_KEY_A, 0);
        if (gc_report.stick_y>128+sticks_sensitivity)
            keyboard->setPressed(HID_KEY_W, 1);  // LEFT STICK UP
        else
            keyboard->setPressed(HID_KEY_W, 0);
        if (gc_report.stick_y<128-sticks_sensitivity)
            keyboard->setPressed(HID_KEY_S, 1);  // LEFT STICK DOWN
        else
            keyboard->setPressed(HID_KEY_S, 0);
        if (gc_report.cstick_x>128+sticks_sensitivity)
            keyboard->setPressed(HID_KEY_KEYPAD_4, 1);  // C STICK LEFT
        else
            keyboard->setPressed(HID_KEY_KEYPAD_4, 0);
        if (gc_report.cstick_x<128-sticks_sensitivity)
            keyboard->setPressed(HID_KEY_KEYPAD_8, 1);  // C STICK RIGHT
        else
            keyboard->setPressed(HID_KEY_KEYPAD_8, 0);
        if (gc_report.cstick_y>128+sticks_sensitivity)
            keyboard->setPressed(HID_KEY_KEYPAD_6, 1);  // C STICK UP
        else
            keyboard->setPressed(HID_KEY_KEYPAD_6, 0);
        if (gc_report.cstick_y<128-sticks_sensitivity)
            keyboard->setPressed(HID_KEY_KEYPAD_5, 1);  // C STICK DOWN
        else
            keyboard->setPressed(HID_KEY_KEYPAD_5, 0);
        if (gc_report.l_analog>triggers_deadzone)
            keyboard->setPressed(HID_KEY_C, 1);  // L2 TRIGGER
        else
            keyboard->setPressed(HID_KEY_C, 0);
        if (gc_report.r_analog>triggers_deadzone)
            keyboard->setPressed(HID_KEY_V, 1);  // R2 TRIGGER
        else
            keyboard->setPressed(HID_KEY_V, 0);

        keyboard->sendState();
    }
}

void setup1() {
    gcc = new GamecubeController(GCC_PIN, 500, pio1);
    poll=1;
}

void loop1() {
    gcc->Poll((gc_report_t *)&gc_report, 0);
}
