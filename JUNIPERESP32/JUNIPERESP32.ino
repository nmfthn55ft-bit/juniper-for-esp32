void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SD.h>
#include <SPI.h>

// ================= TFT =================
TFT_eSPI tft = TFT_eSPI();

// ================= BUTTONS =================
#define BTN_UP     32
#define BTN_DOWN   33
#define BTN_OK     25
#define BTN_BACK   26
#define BTN_NEXT   27

// ================= SYSTEM STATES =================
enum State {
  BOOT,
  FILE_MANAGER
};

State state = BOOT;

// ================= FILE SYSTEM =================
File root;
String files[40];
int fileCount = 0;
int selected = 0;

// ================= INPUT =================
bool pressed(int pin) {
  return digitalRead(pin) == LOW;
}

// ================= BOOT SCREEN =================
void bootScreen() {

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  tft.setCursor(20, 20);
  tft.println("JUNIPER FW");

  tft.setCursor(40, 60);
  tft.println("╱|、");

  tft.setCursor(40, 80);
  tft.println("(˚ˎ 。7");

  tft.setCursor(40, 100);
  tft.println("|、˜〵");

  tft.setCursor(40, 120);
  tft.println("じしˍ,)ノ");

  delay(1200);

  tft.setCursor(20, 160);
  tft.println("Booting...");
  delay(600);

  tft.setCursor(20, 180);
  tft.println("digging holes...");
  delay(600);

  state = FILE_MANAGER;
}

// ================= LOAD FILES =================
void loadFiles() {

  fileCount = 0;

  root = SD.open("/");
  File file = root.openNextFile();

  while (file && fileCount < 40) {
    files[fileCount] = file.name();
    fileCount++;
    file = root.openNextFile();
  }
}

// ================= DRAW FILE MANAGER =================
void drawFileManager() {

  tft.fillScreen(TFT_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);

  tft.setCursor(10, 10);
  tft.println("FILE MANAGER");

  for (int i = 0; i < fileCount; i++) {

    int y = 40 + (i * 18);

    if (y > 220) break;

    if (i == selected) {
      tft.fillRect(5, y - 2, 230, 16, TFT_DARKGREY);
      tft.setTextColor(TFT_BLACK);
      tft.setCursor(10, y);
      tft.print(">");
    } else {
      tft.setTextColor(TFT_WHITE);
      tft.setCursor(10, y);
      tft.print(" ");
    }

    tft.print(files[i]);
  }
}

// ================= OPEN FILE =================
void openFile(int index) {

  String name = files[index];
  File f = SD.open("/" + name);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 10);

  tft.println("OPENING:");
  tft.println(name);
  tft.println("----------------");

  int y = 60;

  while (f.available() && y < 220) {
    String line = f.readStringUntil('\n');
    tft.setCursor(10, y);
    tft.println(line);
    y += 12;
  }

  f.close();

  delay(1500);
  drawFileManager();
}

// ================= SETUP =================
void setup() {

  Serial.begin(115200);

  // buttons
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);

  // TFT
  tft.init();
  tft.setRotation(1);

  // SD card
  if (!SD.begin()) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED);
    tft.setCursor(10, 10);
    tft.println("SD FAIL");
    while (true);
  }

  bootScreen();

  loadFiles();
  drawFileManager();
}

// ================= LOOP =================
void loop() {

  if (state == FILE_MANAGER) {

    if (pressed(BTN_UP)) {
      selected--;
      if (selected < 0) selected = fileCount - 1;
      drawFileManager();
      delay(200);
    }

    if (pressed(BTN_DOWN)) {
      selected++;
      if (selected >= fileCount) selected = 0;
      drawFileManager();
      delay(200);
    }

    if (pressed(BTN_OK)) {
      openFile(selected);
      delay(200);
    }

    if (pressed(BTN_BACK)) {
      loadFiles();
      drawFileManager();
      delay(200);
    }

  }
}