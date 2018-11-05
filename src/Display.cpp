#include "Display.h"

static const TUint32 FRAMERATE = 30;
static TUint32       sNow, sNext;

Display gDisplay;

// The below lines are for enabling editing with some intelligence within CLion
//#ifndef __XTENSA__
//#define __XTENSA__
//#endif

static void nextFrameDelay() {
  while (sNow < sNext) {
    sNow = Milliseconds();
  }
  sNext = (sNext + 1000 / FRAMERATE);
}

#ifdef __XTENSA__

#pragma GCC optimize ("O3")


static const TInt DUTY_MAX = 0x1fff;

static const gpio_num_t SPI_PIN_NUM_MISO = GPIO_NUM_19;
static const gpio_num_t SPI_PIN_NUM_MOSI = GPIO_NUM_23;
static const gpio_num_t SPI_PIN_NUM_CLK  = GPIO_NUM_18;

static const gpio_num_t LCD_PIN_NUM_CS   = GPIO_NUM_5;
static const gpio_num_t LCD_PIN_NUM_DC   = GPIO_NUM_21;
static const gpio_num_t LCD_PIN_NUM_BCKL = GPIO_NUM_14;
static const TInt LCD_BACKLIGHT_ON_VALUE = 1;
static const TInt LCD_SPI_CLOCK_RATE = 40000000;
//static const TInt LCD_SPI_CLOCK_RATE = 80000000;


#define SPI_TRANSACTION_COUNT (4)
static spi_transaction_t trans[SPI_TRANSACTION_COUNT];
static spi_device_handle_t spi_device_handle;


#define LINE_BUFFERS (2)
#define PARALLEL_LINES (5)
static TUint16* line[LINE_BUFFERS];
static QueueHandle_t spi_queue;
static QueueHandle_t line_buffer_queue;

static SemaphoreHandle_t line_semaphore;
static SemaphoreHandle_t spi_empty;
static SemaphoreHandle_t spi_count_semaphore;
static SemaphoreHandle_t displayMutex = NULL;


static TBool backlightInitialized = EFalse;




#define TFT_CMD_SWRESET 0x01
#define TFT_CMD_SLEEP 0x10
#define TFT_CMD_DISPLAY_OFF 0x28

#define MADCTL_MY   0x80
#define MADCTL_MX   0x40
#define MADCTL_MV   0x20
#define MADCTL_ML   0x10
#define MADCTL_MH   0x04
#define TFT_RGB_BGR 0x08
#define TFT_RGB 0x00

/*
 The ILI9341 needs a bunch of command/argument values to be initialized. They are stored in this struct.
*/
typedef struct {
  TUint8 cmd;
  TUint8 data[128];
  TUint8 databytes; //Number of items in data; bit 7 = delay after set; 0xFF = end of cmds.
} ili_init_cmd_t;



// 2.4" LCD
static const ili_init_cmd_t display_init_commands[] = {
  // VCI=2.8V
  //************* Start Initial Sequence **********//
  {TFT_CMD_SWRESET, {0}, 0x80},
  {0xCF, {0x00, 0xc3, 0x30}, 3},
  {0xED, {0x64, 0x03, 0x12, 0x81}, 4},
  {0xE8, {0x85, 0x00, 0x78}, 3},
  {0xCB, {0x39, 0x2c, 0x00, 0x34, 0x02}, 5},
  {0xF7, {0x20}, 1},
  {0xEA, {0x00, 0x00}, 2},
  {0xC0, {0x1B}, 1},  //Power control   //VRH[5:0]
  {0xC1, {0x12}, 1},  //Power control   //SAP[2:0];BT[3:0]
  {0xC5, {0x32, 0x3C}, 2},  //VCM control
  {0xC7, {0x91}, 1},  //VCM control2
  {0x36, {(MADCTL_MV | MADCTL_MY | TFT_RGB_BGR)}, 1},  // Memory Access Control
  {0x3A, {0x55}, 1},
  {0xB1, {0x00, 0x1B}, 2},  // Frame Rate Control (1B=70, 1F=61, 10=119)
  {0xB6, {0x0A, 0xA2}, 2},  // Display Function Control
  {0xF6, {0x01, 0x30}, 2},
  {0xF2, {0x00}, 1},  // 3Gamma Function Disable
  {0x26, {0x01}, 1},   //Gamma curve selected

  //Set Gamma
  {0xE0, {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00}, 15},
  {0XE1, {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F}, 15},

/*
  // LUT
  {0x2d, {0x01, 0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1b, 0x1d, 0x1f,
      0x21, 0x23, 0x25, 0x27, 0x29, 0x2b, 0x2d, 0x2f, 0x31, 0x33, 0x35, 0x37, 0x39, 0x3b, 0x3d, 0x3f,
      0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
      0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c,
      0x1d, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x26, 0x27, 0x28, 0x29, 0x2a,
      0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
      0x00, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x12, 0x14, 0x16, 0x18, 0x1a,
      0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38}, 128},
*/

  {0x11, {0}, 0x80},  //Exit Sleep
  {0x29, {0}, 0x80},  //Display on

  {0, {0}, 0xff}
};


Display::Display() {
  printf("Display::Display()\n"); fflush(stdout);
  mBitmap1      = new BBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, MEMF_FAST);
  mBitmap2      = new BBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, MEMF_FAST);
  renderBitmap  = mBitmap1;
  displayBitmap = mBitmap2;

  sNow  = Milliseconds();
  sNext = sNow;
  sNext = sNext + 1000 / FRAMERATE;
}

Display::~Display() {
  delete mBitmap1;
  delete mBitmap2;
}

//static volatile bool displayLocked = false;
void Display::LockDisplay() {
  if (!displayMutex) {
    displayMutex = xSemaphoreCreateMutex();
    if (!displayMutex) abort();
  }

  if (xSemaphoreTake(displayMutex, 1000 / portTICK_RATE_MS) != pdTRUE) {
    printf("Could not get displayMutex! aborting!()\n"); fflush(stdout);
    abort();
  }
}

void Display::UnlockDisplay() {
  if (!displayMutex) {
    printf("Could not get displayMutex, aborting!\n"); fflush(stdout);
    abort();
  }

  xSemaphoreGive(displayMutex);
}



TUint16* Display::GetLineBufferQueue() {
  TUint16* buffer;
  if (xQueueReceive(line_buffer_queue, &buffer, 1000 / portTICK_RATE_MS) != pdTRUE) {
    abort();
  }

  return buffer;
}

void Display::PutLineBufferQueue(TUint16 *buffer) {
  if (xQueueSend(line_buffer_queue, &buffer, 1000 / portTICK_RATE_MS) != pdTRUE) {
    abort();
  }
}



void Display::SpiTask(void *arg) {
  printf("Display %s: Entered.\n", __func__);

  while(ETrue) {
    // Ensure only LCD transactions are pulled
    if(xSemaphoreTake(spi_count_semaphore, portMAX_DELAY) == pdTRUE ) {
      spi_transaction_t* t;

      esp_err_t ret = spi_device_get_trans_result(spi_device_handle, &t, portMAX_DELAY);
      assert(ret == ESP_OK);

      TInt dc = (TInt)t->user & 0x80;
      
      if (dc) {
        xSemaphoreGive(line_semaphore);
        Display::PutLineBufferQueue((TUint16 *) t->tx_buffer);
      }

      if (xQueueSend(spi_queue, &t, portMAX_DELAY) != pdPASS) {
        abort();
      }

      if (uxQueueMessagesWaiting(spi_queue) >= SPI_TRANSACTION_COUNT) {
        xSemaphoreGive(spi_empty);
      }
    }
    else {
      printf("%s: xSemaphoreTake failed.\n", __func__); fflush(stdout);
    }
  }

  printf("%s: Exiting.\n", __func__);
  vTaskDelete(NULL);

//  while (1) {}
}

void Display::InitializeSPI() {
  spi_queue = xQueueCreate(SPI_TRANSACTION_COUNT, sizeof(void*));
  if(!spi_queue) abort();


  line_buffer_queue = xQueueCreate(LINE_BUFFERS, sizeof(void*));
  if(!line_buffer_queue) abort();

  line_semaphore = xSemaphoreCreateCounting(LINE_BUFFERS, LINE_BUFFERS);
  if (!line_semaphore) abort();

  spi_empty = xSemaphoreCreateBinary();
  if(!spi_empty) abort();

  xSemaphoreGive(spi_empty);

  spi_count_semaphore = xSemaphoreCreateCounting(SPI_TRANSACTION_COUNT, 0);
  if (!spi_count_semaphore) abort();

  xTaskCreatePinnedToCore(
      &Display::SpiTask,
    "SpiTask",
    1024 + 768, 
    NULL, 
    5, 
    NULL, 
    1
  );
}

QueueHandle_t displayQueue;
TaskHandle_t displayTaskHandle;


void Display::DisplayTask(void *arg) {

  BBitmap *bitmap;

  while (1) {

    xQueuePeek(displayQueue, &bitmap, portMAX_DELAY);

    // TODO: Jay - this can be optimized by creating the 565 palette once, and then again only when SetPalette() or
    TUint16 palette[256];
    for (TInt c = 0; c < 256; c++) {
      palette[c] = gDisplay.color565(bitmap->mPalette[c].b, bitmap->mPalette[c].r, bitmap->mPalette[c].g);
    }

    Display::WriteFrame(bitmap->mPixels, palette);
    xQueueReceive(displayQueue, &bitmap, portMAX_DELAY);
  }


  printf("%s: Exiting.\n", __func__);
  vTaskDelete(NULL);

}


spi_transaction_t* Display::GetSpiTransaction() {
  spi_transaction_t* t;
  xQueueReceive(spi_queue, &t, portMAX_DELAY);

  memset(t, 0, sizeof(*t));
  return t;
}

void Display::PutSpiTransaction(spi_transaction_t *t) {
  t->rx_buffer = NULL;
  t->rxlength = t->length;

  if (t->flags & SPI_TRANS_USE_TXDATA)
  {
    t->flags |= SPI_TRANS_USE_RXDATA;
  }

  if (uxSemaphoreGetCount(spi_empty) > 0)
  {
    if(xSemaphoreTake(spi_empty, portMAX_DELAY) != pdTRUE)
    {
      abort();
    }
  }

  esp_err_t ret = spi_device_queue_trans(spi_device_handle, t, portMAX_DELAY);
  assert(ret==ESP_OK);

  xSemaphoreGive(spi_count_semaphore);
}


//Send a command to the ILI9341. Uses spi_device_transmit, which waits until the transfer is complete.
void Display::SendDisplayCommand(const TUint8 cmd) {
  spi_transaction_t* t = Display::GetSpiTransaction();

  t->length = 8;
  t->tx_data[0] = cmd;
  t->user = (void*)0;
  t->flags = SPI_TRANS_USE_TXDATA;

  Display::PutSpiTransaction(t);
}

//Send data to the ILI9341. Uses spi_device_transmit, which waits until the transfer is complete.
void Display::SendDisplayData(const TUint8 *data, int len) {
  if (len) {
    spi_transaction_t* t = Display::GetSpiTransaction();

    if (len < 5) {
      for (TInt i = 0; i < len; ++i) {
        t->tx_data[i] = data[i];
      }
      t->length = len * 8;         //Len is in bytes, transaction length is in bits.
      t->user = (void*)1;        //D/C needs to be set to 1
      t->flags = SPI_TRANS_USE_TXDATA;
    }
    else {
      t->length = len * 8;         //Len is in bytes, transaction length is in bits.
      t->tx_buffer = data;         //Data
      t->user = (void*)1;        //D/C needs to be set to 1
      t->flags = 0; //SPI_TRANS_USE_TXDATA;
    }

    Display::PutSpiTransaction(t);
  }
}

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void Display::SpiPreTransferCallback(spi_transaction_t *t) {
  TInt dc = (TInt)t->user & 0x01;
  gpio_set_level(LCD_PIN_NUM_DC, dc);
}

//Initialize the display
void Display::SendDisplayBootProgram() {
  TInt index = 0;

  //Initialize non-SPI GPIOs
  gpio_set_direction(LCD_PIN_NUM_DC, GPIO_MODE_OUTPUT);
  gpio_set_direction(LCD_PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

  //Send all the commands
  while (display_init_commands[index].databytes != 0xff) {
    Display::SendDisplayCommand(display_init_commands[index].cmd);

    TInt len = display_init_commands[index].databytes & 0x7f;
    if (len) Display::SendDisplayData(display_init_commands[index].data, len);

    if (display_init_commands[index].databytes & 0x80) {
      vTaskDelay(100 / portTICK_RATE_MS);
    }

    index++;
  }
}


void Display::SendResetDrawing(TUint8 left, TUint8 top, TUint16 width, TUint8 height) {
  // printf("%s: left:%i, top:%i, width:%i, height:%i\n", __func__, left, top, width, height);
  // fflush(stdout);

  Display::SendDisplayCommand(0x2A); // Column Address Set

  // Casts deal with: error: narrowing conversion of '(((((int)left) + ((int)width)) + -1) >> 8)' from 'int' to
  // 'TUint8 {aka unsigned char}' inside { } [-Werror=narrowing]
  const TUint8 data1[] = { 
    (TUint8)(left >> 8), 
    (TUint8)(left & 0xff), 
    (TUint8)((left + width - 1) >> 8), 
    (TUint8)((left + width - 1) & 0xff) 
  };

  Display::SendDisplayData(data1, 4);

  Display::SendDisplayCommand(0x2B);      //Page address set

  const TUint8 data2[] = { 
    (TUint8)(top >> 8), 
    (TUint8)(top & 0xff), 
    (TUint8)((top + height - 1) >> 8), 
    (TUint8)((top + height - 1) & 0xff) 
  };
  Display::SendDisplayData(data2, 4);

  Display::SendDisplayCommand(0x2C);       //memory write
}

void Display::SendContinueWait() {
  if (xSemaphoreTake(spi_empty, 1000 / portTICK_RATE_MS) != pdTRUE )  {
    abort();
  }
}

void Display::SendContinueLine(TUint16 *line, TInt width, TInt lineCount) {
  spi_transaction_t* t;

  t = Display::GetSpiTransaction();

  t->tx_data[0] = 0x3C;   //memory write continue
  t->length = 8;
  t->user = (void*)0;
  t->flags = SPI_TRANS_USE_TXDATA;

  Display::PutSpiTransaction(t);


  t = Display::GetSpiTransaction();

  t->length = width * 2 * lineCount * 8;
  t->tx_buffer = line;
  t->user = (void*)0x81;
  t->flags = 0;

  Display::PutSpiTransaction(t);
}

void Display::InitializeBacklight() {
  // Note: In esp-idf v3.0, settings flash speed to 80Mhz causes the LCD controller
  // to malfunction after a soft-reset.

  //configure timer0
  ledc_timer_config_t ledc_timer;
  memset(&ledc_timer, 0, sizeof(ledc_timer));

//  ledc_timer.bit_num = LEDC_TIMER_13_BIT; //set timer counter bit number
  ledc_timer.duty_resolution = LEDC_TIMER_13_BIT; //set timer counter bit number
  ledc_timer.freq_hz = 5000;        //set frequency of pwm
  ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;   //timer mode,
  ledc_timer.timer_num = LEDC_TIMER_0;  //timer index


  ledc_timer_config(&ledc_timer);


  //set the configuration
  ledc_channel_config_t ledc_channel;
  memset(&ledc_channel, 0, sizeof(ledc_channel));

  //set LEDC channel 0
  ledc_channel.channel = LEDC_CHANNEL_0;
  //set the duty for initialization.(duty range is 0 ~ ((2**bit_num)-1)
  ledc_channel.duty = (LCD_BACKLIGHT_ON_VALUE) ? 0 : DUTY_MAX;
  //GPIO number
  ledc_channel.gpio_num = LCD_PIN_NUM_BCKL;
  //GPIO INTR TYPE, as an example, we enable fade_end interrupt here.
  ledc_channel.intr_type = LEDC_INTR_FADE_END;
  //set LEDC mode, from ledc_mode_t
  ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
  //set LEDC timer source, if different channel use one timer,
  //the frequency and bit_num of these channels should be the same
  ledc_channel.timer_sel = LEDC_TIMER_0;


  ledc_channel_config(&ledc_channel);


  //initialize fade service.
  ledc_fade_func_install(0);

  // duty range is 0 ~ ((2**bit_num)-1)
  ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (LCD_BACKLIGHT_ON_VALUE) ? DUTY_MAX : 0, 500);
  ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);

  backlightInitialized = ETrue;
}


void Display::Init() {
//  printf("Display::Init(%p)\n", mBitmap1);fflush(stdout);
  Display::InitializeSPI();

  // Malloc the buffers used to paint the display via SPI transactions
  const size_t bufferSize = DISPLAY_WIDTH * PARALLEL_LINES * sizeof(TUint16);
  line[0] = (TUint16 *)heap_caps_malloc(bufferSize, MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
  if (!line[0]) {
    abort();
  }

//  printf("%s: malloc line[0] with size %i bytes\n", __func__, bufferSize);
  Display::PutLineBufferQueue(line[0]);

  line[1] = (TUint16 *)heap_caps_malloc(bufferSize, MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
  if (!line[0]) {
    abort();
  }

//  printf("%s: malloc line[1] with size %i bytes\n", __func__, bufferSize);
  Display::PutLineBufferQueue(line[1]);
  if (!line[1]) {
    abort();
  }


  // Initialize transactions
  for (TInt x = 0; x < SPI_TRANSACTION_COUNT; x++) {
    void* param = &trans[x];
    xQueueSend(spi_queue, &param, portMAX_DELAY);
  }


  // Initialize SPI
  esp_err_t ret;
  spi_bus_config_t buscfg;
  memset(&buscfg, 0, sizeof(buscfg));

  buscfg.miso_io_num = SPI_PIN_NUM_MISO;
  buscfg.mosi_io_num = SPI_PIN_NUM_MOSI;
  buscfg.sclk_io_num = SPI_PIN_NUM_CLK;
  buscfg.quadwp_io_num = -1;
  buscfg.quadhd_io_num = -1;

  spi_device_interface_config_t devcfg;
  memset(&devcfg, 0, sizeof(devcfg));

  devcfg.clock_speed_hz = LCD_SPI_CLOCK_RATE;
  devcfg.mode = 0;                //SPI mode 0
  devcfg.spics_io_num = LCD_PIN_NUM_CS;         //CS pin
  devcfg.queue_size = 7;              //We want to be able to queue 7 transactions at a time
  devcfg.pre_cb = Display::SpiPreTransferCallback;  //Specify pre-transfer callback to handle D/C line
  devcfg.flags = SPI_DEVICE_NO_DUMMY; //SPI_DEVICE_HALFDUPLEX;

  //Initialize the SPI bus
  ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
  assert(ret==ESP_OK);

  //Attach the LCD to the SPI bus
  ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi_device_handle);
  assert(ret==ESP_OK);

  //Initialize the LCD
//  printf("LCD: calling Display::SendDisplayBootProgram().\n");
  Display::SendDisplayBootProgram();

//  printf("LCD: calling Display::InitializeBacklight.\n");
  Display::InitializeBacklight();

//  printf("LCD Initialized (%d Hz).\n", LCD_SPI_CLOCK_RATE);

  displayQueue = xQueueCreate(1, sizeof(uint16_t*));


  xTaskCreatePinnedToCore(&Display::DisplayTask, "DisplayTask", 1024 * 4, NULL, 5, &displayTaskHandle, 1);
}


void Display::WriteFrame(TUint8* frameBuffer, TUint16* palette) {
  short y;

  Display::LockDisplay();

  const TUint16 displayWidth = DISPLAY_WIDTH;
  Display::SendResetDrawing(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  TUint16 step = (DISPLAY_WIDTH * sizeof(TUint16) * PARALLEL_LINES) >> 1;
  uint32_t position = 0, 
           end = 0;

  TUint8* bufferPointer = frameBuffer;


  for (y = 0; y < DISPLAY_HEIGHT; y += PARALLEL_LINES) {
    TUint16* line_buffer = Display::GetLineBufferQueue();

    end += step;

    for (TUint16 i = 0; i < step; i++) {
      line_buffer[i] = palette[bufferPointer[position++]];
    }

    Display::SendContinueLine(line_buffer, displayWidth, PARALLEL_LINES);
  }

  Display::SendContinueWait();
  Display::UnlockDisplay();
}


void Display::Clear(TUint16 color) {
  Display::SendResetDrawing(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  // Clear the buffer
  for (TInt i = 0; i < LINE_BUFFERS; ++i){
    for (TInt j = 0; j < DISPLAY_WIDTH * PARALLEL_LINES; ++j) {
      line[i][j] = color;
    }
  }

  // Clear the screen
  Display::SendResetDrawing(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  for (TInt y = 0; y < DISPLAY_HEIGHT; y += PARALLEL_LINES) {
    TUint16* line_buffer = Display::GetLineBufferQueue();
    Display::SendContinueLine(line_buffer, DISPLAY_WIDTH, PARALLEL_LINES);
  }

  Display::SendContinueWait();
}



void Display::Update() {
//  if (displayLocked) {
//    nextFrameDelay();
//    return;
//  }


  //NOTE: Keep this here! Do not move this xQueueSend line below the swap logic below.
  // swap display and render bitmaps
  if (renderBitmap == mBitmap1) {
    renderBitmap = mBitmap2;
    displayBitmap = mBitmap1;
  }
  else {
    renderBitmap = mBitmap1;
    displayBitmap = mBitmap2;
  }
  xQueueSend(displayQueue, &displayBitmap, portMAX_DELAY);


  // Throttle to FRAMERATE
  nextFrameDelay();
}

#else

#include "SDL2/SDL.h"

static SDL_Window   *screen   = nullptr;
static SDL_Renderer *renderer = nullptr;
static SDL_Texture  *texture  = nullptr;




Display::Display() {
  sNow  = Milliseconds();
  sNext = sNow;
  sNext = sNext + 1000 / FRAMERATE;

  // initialize any hardware
  SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

  // Create an application window with the following settings:
  screen = SDL_CreateWindow(
    "creative-engine",                  // window title
    SDL_WINDOWPOS_UNDEFINED,           // initial resources position
    SDL_WINDOWPOS_UNDEFINED,           // initial y position
    SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2,   // width, in pixels
    SDL_WINDOW_OPENGL                  // flags - see below
  );

  // Check that the window was successfully created
  if (screen == nullptr) {
    // In the case that the window could not be made...
    printf("Could not create window: %s\n", SDL_GetError());
    exit(1);
  } else {
    TInt w, h;
    SDL_GL_GetDrawableSize(screen, &w, &h);
    printf("widthxheight: %dx%d\n", w, h);
  }

  renderer = SDL_CreateRenderer(screen, -1, 0);
  if (!renderer) {
    printf("Cannot create renderer %s\n", SDL_GetError());
    exit(1);
  }
  SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
  SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,
                              SCREEN_HEIGHT);
  if (!texture) {
    printf("Cannot create texturre %s\n", SDL_GetError());
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_GL_SetSwapInterval(1);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  mBitmap1      = new BBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, MEMF_FAST);
  mBitmap2      = new BBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, MEMF_FAST);
  renderBitmap  = mBitmap1;
  displayBitmap = mBitmap2;
}

Display::~Display() {
  // Close and destroy the window
  SDL_DestroyWindow(screen);

  // Clean up
  SDL_Quit();
  delete mBitmap1;
  delete mBitmap2;
  SDL_DestroyWindow(screen);
  SDL_Quit();
}

void Display::Init() {
  // For compatability 
}

void Display::Update() {
  // swap display and render bitmaps
  if (renderBitmap == mBitmap1) {
    renderBitmap  = mBitmap2;
    displayBitmap = mBitmap1;
  } else {
    renderBitmap  = mBitmap1;
    displayBitmap = mBitmap2;
  }

  void *screenBuf;
  TInt pitch;

  if (0 == SDL_LockTexture(texture, nullptr, &screenBuf, &pitch)) {
    auto        *screenBits = (TUint32 *) screenBuf;
    TRGB        *palette    = displayBitmap->mPalette;
    for (TInt16 y           = 0; y < SCREEN_HEIGHT; y++) {
      TUint8    *ptr = &displayBitmap->mPixels[y * displayBitmap->mPitch];
      for (TInt x    = 0; x < SCREEN_WIDTH; x++) {
        TUint8  pixel = *ptr++;
        TUint32 color = palette[pixel].rgb888();
        *screenBits++ = color;
      }
    }

//    (TUint32 *) screenBuf;
//    screenBits = (TUint32 *) screenBuf;
//    Dump(screenBits, renderBitmap->mWidth);
//    Dump(displayBitmap->mPixels, displayBitmap->mWidth, displayBitmap->mHeight);
    SDL_UnlockTexture(texture);
  } else {
    printf("Can't lock texture (%s)\n", SDL_GetError());
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr); // Render texture to entire window
  SDL_RenderPresent(renderer);              // Do update

  nextFrameDelay();
}

#endif
