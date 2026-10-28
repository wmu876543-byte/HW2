
/*
  Interactive Toy - HW2
  Board: Arduino Uno / Nano

  RGB LED: Common Anode (共陽極)
  Button: Active LOW (按下為 LOW)

  功能：
  1. RGB LED 顯示玩具心情
  2. 按下按鈕模擬撫摸，增加心情點數
  3. 長時間沒有互動，心情逐漸下降
  4. 心情越低，LED 顏色越偏向傷心狀態
*/

const int RED_PIN   = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN  = 11;
const int BUTTON_PIN = 2;

// 心情設定
int mood = 50;
const int MAX_MOOD = 100;
const int MIN_MOOD = 0;

const int PET_GAIN = 5;          // 每次撫摸增加的點數
const int SAD_LOSS = 1;           // 每次扣除的點數

const unsigned long SAD_INTERVAL = 100;
// 0.2 秒沒有互動，心情下降一次

const unsigned long DEBOUNCE_MS = 30;

unsigned long lastInteraction = 0;
unsigned long lastSadUpdate = 0;
unsigned long lastDebounceTime = 0;

int lastButtonReading = HIGH;
int stableButtonState = HIGH;


// 共陽極 RGB LED：PWM 值必須反轉
void setColor(int red, int green, int blue) {
  analogWrite(RED_PIN,   255 - red);
  analogWrite(GREEN_PIN, 255 - green);
  analogWrite(BLUE_PIN,  255 - blue);
}


// 根據心情顯示顏色

void updateLED() {
  int red, green, blue;

  if (mood <= 50) {
    // 心情 0 → 50：紅色逐漸過渡到藍色
    red   = map(mood, 0, 50, 255, 0);
    green = 0;
    blue  = map(mood, 0, 50, 0, 255);
  } else {
    // 心情 50 → 100：藍色逐漸過渡到綠色
    red   = 0;
    green = map(mood, 50, 100, 0, 255);
    blue  = map(mood, 50, 100, 255, 0);
  }

  // 共陽極 LED：setColor() 內部已經反轉 PWM
  setColor(red, green, blue);
}


// 撫摸玩具
void petToy() {
  mood += PET_GAIN;

  if (mood > MAX_MOOD) {
    mood = MAX_MOOD;
  }

  lastInteraction = millis();
  lastSadUpdate = millis();

  Serial.print("Pet! Mood = ");
  Serial.println(mood);

  updateLED();
}


void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // 按鈕一端接 D2，另一端接 GND
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(9600);

  lastInteraction = millis();
  lastSadUpdate = millis();

  updateLED();

  Serial.println("Interactive Toy Ready!");
}


void loop() {
  unsigned long now = millis();

  // ---------- 讀取按鈕 ----------
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = now;
  }

  if (now - lastDebounceTime >= DEBOUNCE_MS) {
    if (reading != stableButtonState) {
      stableButtonState = reading;

      // 只在按下的瞬間觸發一次
      if (stableButtonState == LOW) {
        petToy();
      }
    }
  }

  lastButtonReading = reading;


  // ---------- 長時間沒有互動 ----------
  if (now - lastInteraction >= SAD_INTERVAL &&
      now - lastSadUpdate >= SAD_INTERVAL) {

    mood -= SAD_LOSS;

    if (mood < MIN_MOOD) {
      mood = MIN_MOOD;
    }

    lastSadUpdate = now;

    Serial.print("No petting! Mood = ");
    Serial.println(mood);

    updateLED();
  }
}
