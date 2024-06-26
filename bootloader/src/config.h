#pragma once

// HID Bootloader takes 4K
#define USER_PROGRAM 0x08002000

#define RTC_BOOTLOADER_FLAG 0x7662 /* Flag whether to jump into bootloader, "vb" */
#define RTC_INSECURE_FLAG 0x4953 /* Flag to indicate qmk that we want to boot into insecure mode, "IS" */

#if defined(TARGET_GENERIC)
#define VIAL_KEYBOARD_UID {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
/* setup for bluepill */
#define BL_OUTPUT_BANK GPIOB
#define BL_OUTPUT_PIN 12
#define BL_INPUT_BANK GPIOB
#define BL_INPUT_PIN 13
#elif defined(TARGET_VIAL_TEST)
#define VIAL_KEYBOARD_UID {0x6F, 0xC5, 0xEE, 0x60, 0x90, 0x92, 0x53, 0xA4}
/* setup for bluepill */
#define BL_OUTPUT_BANK GPIOB
#define BL_OUTPUT_PIN 12
#define BL_INPUT_BANK GPIOB
#define BL_INPUT_PIN 13
#elif defined(TARGET_SR_STUDIO)
/*
  This should match the UID you have configured in the Vial firmware
  See Vial porting docs for more info: https://vial-kb.github.io/gettingStarted/porting-to-vial.html
*/
#define VIAL_KEYBOARD_UID {0xD0, 0x46, 0x6F, 0xC7, 0x8C, 0x50, 0x6A, 0x15}
/*
  These configure an emergency key held to trigger bootloader entry on boot.
  See https://github.com/xyzz/stm32f103-keyboard-bootloader#adding-a-new-keyboard for an explanation.
*/
#define BL_OUTPUT_BANK GPIOB
#define BL_OUTPUT_PIN 8
#define BL_INPUT_BANK GPIOB
#define BL_INPUT_PIN 3
#else
#error Unknown target
#endif
