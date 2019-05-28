// A lot of this code comes from  https://github.com/flosse/linuxconsole/blob/master/utils/jstest.c
// License GPL2 https://github.com/flosse/linuxconsole/blob/master/COPYING
#ifdef __MODUS_TARGET_DIRECT_LINUX_CONTROLS__

#include "DirectLinuxControls.h"

#define DIRECT_CONTROLLER_AXIS_MAX 32767
#define DIRECT_CONTROLLER_AXIS_OFF 0
#define DIRECT_CONTROLLER_BUTTON_OFF 0
#define DIRECT_CONTROLLER_BUTTON_ON 0


typedef enum {
  DIRECT_CONTROLLER_DPAD_LEFT_RIGHT = 0,
  DIRECT_CONTROLLER_DPAD_UP_DOWN
} Direct_Controller_DPad_Type;

typedef enum {
  DIRECT_CONTROLLER_INPUT_TYPE_BUTTON = 1,
  DIRECT_CONTROLLER_INPUT_TYPE_DPAD
} Direct_Controller_Input_Type;

typedef enum {
  DIRECT_CONTROLLER_BUTTON_X = 0,
  DIRECT_CONTROLLER_BUTTON_A,
  DIRECT_CONTROLLER_BUTTON_B,
  DIRECT_CONTROLLER_BUTTON_Y,
  DIRECT_CONTROLLER_BUTTON_UNK1,
  DIRECT_CONTROLLER_BUTTON_UNK2,
  DIRECT_CONTROLLER_BUTTON_L,
  DIRECT_CONTROLLER_BUTTON_R,
  DIRECT_CONTROLLER_BUTTON_SELECT,
  DIRECT_CONTROLLER_BUTTON_START,
} Direct_Controller_Button_Type;

typedef enum {
  DIRECT_CONTROLLER_BUTTON_RELEASED = 0,
  DIRECT_CONTROLLER_BUTTON_PRESSED
} Direct_Controller_Button_Press_Type;

#define NAME_LENGTH 128


DirectLinuxControls::DirectLinuxControls() : ControlsBase() {
#ifdef __linux__
  pthread_mutex_destroy(&mMutex);
  pthread_mutex_init(&mMutex, NULL);
#endif
  StartThread();
}


void DirectLinuxControls::PollingThread(DirectLinuxControls *myControls) {
//  printf("PollingThread()\n");

  int fd, i;
  unsigned char axes = 2;
  unsigned char buttons = 2;
  int version = 0x000800;
  char name[NAME_LENGTH] = "Unknown";
  uint16_t btnmap[BTNMAP_SIZE];
  uint8_t axmap[AXMAP_SIZE];
  int btnmapok = 1;

  const char *joystickIdentifier = "/dev/input/js0";

  if ((fd = open(joystickIdentifier, O_RDONLY)) < 0) {
    printf("Could not open %s!!!\n", joystickIdentifier);
    return;
  }

  ioctl(fd, JSIOCGVERSION, &version);
  ioctl(fd, JSIOCGAXES, &axes);
  ioctl(fd, JSIOCGBUTTONS, &buttons);
  ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);

  getaxmap(fd, axmap);
  getbtnmap(fd, btnmap);

//  printf("Driver version is %d.%d.%d.\n",
//         version >> 16, (version >> 8) & 0xff, version & 0xff);

  /* Determine whether the button map is usable. */
  for (i = 0; btnmapok && i < buttons; i++) {
    if (btnmap[i] < BTN_MISC || btnmap[i] > KEY_MAX) {
      btnmapok = 0;
      break;
    }
  }

  if (!btnmapok) {
    /* btnmap out of range for names. Don't print any. */
    puts("jstest is not fully compatible with your kernel. Unable to retrieve button map!");
    printf("Joystick (%s) has %d axes ", name, axes);
    printf("and %d buttons.\n", buttons);
    printf("CONTROLLER WILL NOT WORK!!!");
    return;
  }

  while (mThreadRunning) {
    struct js_event js;
    fcntl(fd, F_SETFL, O_NONBLOCK);

    while(read(fd, &js, sizeof(struct js_event)) == sizeof(struct js_event)) {
//      printf("Event: type %d, time %d, number %d, value %d\n",
//             js.type, js.time, js.number, js.value);
//
//      fflush(stdout);
      myControls->ParseEvent(&js);

      if (! mThreadRunning) {
        break;
      }
    }
//    usleep(1);
  }

  printf("Polling thread ended!\n");
}

DirectLinuxControls::~DirectLinuxControls() {
  StopThread();
}

void DirectLinuxControls::ParseEvent(struct js_event *event) {

  if (event->type > 2) {
    return;
  }

  TUint16 keys = bKeys;

  if (event->type == DIRECT_CONTROLLER_INPUT_TYPE_DPAD) {
    // Up - Down
    if (event->number == DIRECT_CONTROLLER_DPAD_UP_DOWN) {
      switch(event->value) {
        case -DIRECT_CONTROLLER_AXIS_MAX:
          keys |= JOYUP;
          break;
        case DIRECT_CONTROLLER_AXIS_MAX:
          keys |= JOYDOWN;
          break;
        case DIRECT_CONTROLLER_AXIS_OFF:
          keys &= ~JOYUP;
          keys &= ~JOYDOWN;
        default:
          break;
      }
    }

    // Left - Right
    if (event->number == DIRECT_CONTROLLER_DPAD_LEFT_RIGHT) {
      switch(event->value) {
        case -DIRECT_CONTROLLER_AXIS_MAX:
          keys |= JOYLEFT;
          break;
        case DIRECT_CONTROLLER_AXIS_MAX:
          keys |= JOYRIGHT;
          break;
        case DIRECT_CONTROLLER_AXIS_OFF:
          keys &= ~JOYLEFT;
          keys &= ~JOYRIGHT;
          break; 
        default:
          break;
      }
    }
  }

  // Buttons
  if (event->type == DIRECT_CONTROLLER_INPUT_TYPE_BUTTON) {
    if (event->value == DIRECT_CONTROLLER_BUTTON_PRESSED) {
      switch (event->number) {
        case DIRECT_CONTROLLER_BUTTON_A:
          keys |= BUTTONB;
          break;
        case DIRECT_CONTROLLER_BUTTON_B:
          keys |= BUTTONA;
          break;
        case DIRECT_CONTROLLER_BUTTON_X:
          keys |= BUTTON_MENU;
          break;
        case DIRECT_CONTROLLER_BUTTON_Y:
          keys |= BUTTON_SOUND;
          break;
        case DIRECT_CONTROLLER_BUTTON_SELECT:
          keys |= BUTTON_SELECT;
          break;
        case DIRECT_CONTROLLER_BUTTON_START:
          keys |= BUTTON_START;
          break;
      }
    }

    if (event->value == DIRECT_CONTROLLER_BUTTON_RELEASED) {
      switch (event->number) {
        case DIRECT_CONTROLLER_BUTTON_A:
          keys &= ~BUTTONB;
          break;
        case DIRECT_CONTROLLER_BUTTON_B:
          keys &= ~BUTTONA;
          break;
        case DIRECT_CONTROLLER_BUTTON_X:
          keys &= ~BUTTON_MENU;
          break;
        case DIRECT_CONTROLLER_BUTTON_Y:
          keys &= ~BUTTON_SOUND;
          break;
        case DIRECT_CONTROLLER_BUTTON_SELECT:
          keys &= ~BUTTON_SELECT;
          break;
        case DIRECT_CONTROLLER_BUTTON_START:
          keys &= ~BUTTON_START;
          break;
      }
    }
    
    
  }

  dKeys |= (keys ^ cKeys) & keys;
  cKeys        = keys;
  bKeys        = keys;
}

TBool DirectLinuxControls::Poll()  {
  return false;
}

#endif