#include <Keypad.h>
#include <LiquidCrystal.h>
#include <math.h>

// تعريف شاشة LCD (بدون I2C)
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// تعريف لوحة المفاتيح 5x4
const byte ROWS = 5;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'F', 'G', '#', '*'},    // [F1] [F2] [#] [*]
  {'1', '2', '3', 'U'},    // [1]  [2]  [3] [↑]
  {'4', '5', '6', 'D'},    // [4]  [5]  [6] [↓]
  {'7', '8', '9', 'E'},    // [7]  [8]  [9] [ESC]
  {'L', '0', 'R', 'N'}     // [←]  [0]  [→] [ENT]
};

byte rowPins[ROWS] = {10, 9, 8, 7, 6};        // دبابيس الصفوف
byte colPins[COLS] = {13, 18, 19, 17};        // دبابيس الأعمدة (13, A4, A5, A3 كرقمية)

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// تعريف الأوضاع
enum Mode {
  MODE_SELECTION,
  BASIC,
  SCIENTIFIC,
  UNIT_CONVERTER,
  CURRENCY_CONVERTER,
  GAME
};

Mode currentMode = MODE_SELECTION;
int selectedMode = 0;

// تعريف أنواع التحويل
enum UnitType {
  KG_TO_G,
  G_TO_KG,
  KM_TO_M,
  M_TO_KM,
  C_TO_F,
  F_TO_C
};

UnitType currentUnitType = KG_TO_G;

// تعريف أنواع العملات
enum CurrencyType {
  USD_TO_EGP,
  EUR_TO_EGP,
  GBP_TO_EGP,
  SAR_TO_EGP
};

CurrencyType currentCurrencyType = USD_TO_EGP;

// متغيرات النظام
String input = "";
String expression = "";
float result = 0;
bool newInput = true;
bool powerMode = false; // للوضع العلمي F+* combo

// متغيرات اللعبة
int gameScore = 0;
int gameAttempts = 3;
int correctAnswer = 0;

// إعلان الدوال مسبقًا
void showModeSelection();
void handleKeyPress(char key);
void handleModeSelection(char key);
void confirmModeSelection();
void returnToModeSelection();
void initializeMode();
void handleBasicMode(char key);
void handleScientificMode(char key);
void handleUnitConverterMode(char key);
void handleCurrencyConverterMode(char key);
void handleGameMode(char key);
void startGame();
void generateProblem();
void updateGameDisplay();
void checkAnswer();
void gameOver();
bool checkDoublePress();
void handleOperator(char op);
void appendOperator(char op);
void calculate();
void evaluateExpression();
void processInput(char key);
void clearAll();
void updateDisplay();
void showError(const char* message);
bool isDigit(char c);
void convertUnits();
void convertCurrency();

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");

  // تهيئة شاشة LCD
  lcd.begin(16, 2);
  delay(100); // تأخير لضمان التهيئة
  Serial.println("LCD Initialized");
  lcd.clear();
  Serial.println("LCD Cleared");

  // عرض شاشة الترحيب
  lcd.setCursor(0, 0);
  lcd.print("Advanced Calc 5x4");
  lcd.setCursor(0, 1);
  lcd.print("Ver 3.0");
  Serial.println("Welcome Screen Displayed");
  delay(2000);

  showModeSelection();
  Serial.println("Mode Selection Displayed");
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    handleKeyPress(key);
  }
}

void handleKeyPress(char key) {
  switch (currentMode) {
    case MODE_SELECTION:
      handleModeSelection(key);
      break;
    case BASIC:
      handleBasicMode(key);
      break;
    case SCIENTIFIC:
      handleScientificMode(key);
      break;
    case UNIT_CONVERTER:
      handleUnitConverterMode(key);
      break;
    case CURRENCY_CONVERTER:
      handleCurrencyConverterMode(key);
      break;
    case GAME:
      handleGameMode(key);
      break;
  }
}

// ============= دوال اختيار الوضع =============
void showModeSelection() {
  lcd.clear();
  delay(50);
  Serial.println("Clearing LCD");
  lcd.setCursor(0, 0);
  lcd.print("Select Mode:");
  Serial.println("Printed 'Select Mode:'");
  
  lcd.setCursor(0, 1);
  switch (selectedMode) {
    case 0: lcd.print("1.Basic Mode"); Serial.println("Printed Basic Mode"); break;
    case 1: lcd.print("2.Scientific"); Serial.println("Printed Scientific"); break;
    case 2: lcd.print("3.Unit Conv"); Serial.println("Printed Unit Conv"); break;
    case 3: lcd.print("4.Currency"); Serial.println("Printed Currency"); break;
    case 4: lcd.print("5.Game"); Serial.println("Printed Game"); break;
  }
}

void handleModeSelection(char key) {
  if (key == 'U') { // ↑
    selectedMode = (selectedMode + 1) % 5;
    showModeSelection();
  } else if (key == 'D') { // ↓
    selectedMode = (selectedMode - 1 + 5) % 5;
    showModeSelection();
  } else if (key == 'N' || key == '#') { // ENT or #
    confirmModeSelection();
  }
}

void confirmModeSelection() {
  switch (selectedMode) {
    case 0: currentMode = BASIC; break;
    case 1: currentMode = SCIENTIFIC; break;
    case 2: currentMode = UNIT_CONVERTER; break;
    case 3: currentMode = CURRENCY_CONVERTER; break;
    case 4:
      currentMode = GAME;
      startGame();
      return;
  }
  initializeMode();
}

void returnToModeSelection() {
  currentMode = MODE_SELECTION;
  showModeSelection();
}

void initializeMode() {
  lcd.clear();
  delay(50);
  lcd.setCursor(0, 0);
  
  switch (currentMode) {
    case BASIC:
      lcd.print("Basic Mode");
      lcd.setCursor(0, 1);
      lcd.print("F+ G- *x #/ ENT=");
      Serial.println("Basic Mode Initialized");
      break;
    case SCIENTIFIC:
      lcd.print("Scientific Mode");
      lcd.setCursor(0, 1);
      lcd.print("F+ G- *x #/ F^");
      Serial.println("Scientific Mode Initialized");
      break;
    case UNIT_CONVERTER:
      lcd.print("Unit Converter");
      lcd.setCursor(0, 1);
      lcd.print("U/D:Type ENT:Conv");
      Serial.println("Unit Converter Initialized");
      break;
    case CURRENCY_CONVERTER:
      lcd.print("Currency Conv");
      lcd.setCursor(0, 1);
      lcd.print("U/D:Type ENT:Conv");
      Serial.println("Currency Converter Initialized");
      break;
    case GAME:
      lcd.print("Math Game");
      Serial.println("Game Mode Initialized");
      break;
  }
  delay(1500);
  clearAll();
  updateDisplay();
}

// ============= دوال الوضع الأساسي =============
void handleBasicMode(char key) {
  Serial.print("Key pressed in Basic Mode: ");
  Serial.println(key);
  
  if (key == 'F') { handleOperator('+'); }
  else if (key == 'G') { handleOperator('-'); }
  else if (key == '*') { handleOperator('*'); }
  else if (key == '#') { 
    if (checkDoublePress()) return;
    handleOperator('/'); 
  }
  else if (key == 'N') { calculate(); }
  else if (key == 'E') { clearAll(); }
  else if (key == 'L') { processInput('.'); } // ← for decimal
  else if (isDigit(key)) { processInput(key); }
}

// ============= دوال الوضع العلمي =============
void handleScientificMode(char key) {
  Serial.print("Key pressed in Scientific Mode: ");
  Serial.println(key);
  
  if (key == 'F') {
    if (powerMode) {
      appendOperator('^');
      powerMode = false;
    } else {
      appendOperator('+');
    }
  } else if (key == 'G') {
    appendOperator('-');
  } else if (key == '*') {
    if (powerMode) {
      appendOperator('^');
      powerMode = false;
    } else {
      appendOperator('*');
    }
  } else if (key == '#') {
    if (checkDoublePress()) return;
    appendOperator('/');
  } else if (key == 'N') {
    evaluateExpression();
    powerMode = false;
  } else if (key == 'E') {
    clearAll();
    powerMode = false;
  } else if (key == 'L') {
    processInput('.'); // ← for decimal
  } else if (isDigit(key)) {
    processInput(key);
  }
}

// ============= دوال تحويل الوحدات =============
void handleUnitConverterMode(char key) {
  if (key == 'U') { // ↑
    currentUnitType = (UnitType)((currentUnitType + 1) % 6);
    updateDisplay();
  } else if (key == 'D') { // ↓
    currentUnitType = (UnitType)((currentUnitType - 1 + 6) % 6);
    updateDisplay();
  } else if (key == 'N') { // ENT
    convertUnits();
  } else if (key == '#') {
    if (checkDoublePress()) return;
  } else if (key == 'E') { // ESC
    clearAll();
  } else if (isDigit(key) || key == 'L') { // ← for decimal
    processInput(key == 'L' ? '.' : key);
  }
}

void convertUnits() {
  if (input.length() == 0) return;
  
  float value = input.toFloat();
  
  switch (currentUnitType) {
    case KG_TO_G: result = value * 1000; break;
    case G_TO_KG: result = value / 1000; break;
    case KM_TO_M: result = value * 1000; break;
    case M_TO_KM: result = value / 1000; break;
    case C_TO_F: result = (value * 9 / 5) + 32; break;
    case F_TO_C: result = (value - 32) * 5 / 9; break;
  }
  
  input = String(result);
  newInput = true;
  updateDisplay();
}

// ============= دوال تحويل العملات =============
void handleCurrencyConverterMode(char key) {
  if (key == 'U') { // ↑
    currentCurrencyType = (CurrencyType)((currentCurrencyType + 1) % 4);
    updateDisplay();
  } else if (key == 'D') { // ↓
    currentCurrencyType = (CurrencyType)((currentCurrencyType - 1 + 4) % 4);
    updateDisplay();
  } else if (key == 'N') { // ENT
    convertCurrency();
  } else if (key == '#') {
    if (checkDoublePress()) return;
  } else if (key == 'E') { // ESC
    clearAll();
  } else if (isDigit(key) || key == 'L') { // ← for decimal
    processInput(key == 'L' ? '.' : key);
  }
}

void convertCurrency() {
  if (input.length() == 0) return;
  
  float value = input.toFloat();
  
  switch (currentCurrencyType) {
    case USD_TO_EGP: result = value * 30.90; break;
    case EUR_TO_EGP: result = value * 33.50; break;
    case GBP_TO_EGP: result = value * 39.20; break;
    case SAR_TO_EGP: result = value * 8.24; break;
  }
  
  input = String(result);
  newInput = true;
  updateDisplay();
}

// ============= دوال اللعبة =============
void startGame() {
  gameScore = 0;
  gameAttempts = 3;
  generateProblem();
}

void generateProblem() {
  lcd.clear();
  delay(50);
  int a = random(1, 10);
  int b = random(1, 10);
  int op = random(0, 4);
  
  switch (op) {
    case 0:
      correctAnswer = a + b;
      lcd.print(String(a) + "+" + String(b) + "=?");
      break;
    case 1:
      correctAnswer = a - b;
      lcd.print(String(a) + "-" + String(b) + "=?");
      break;
    case 2:
      correctAnswer = a * b;
      lcd.print(String(a) + "*" + String(b) + "=?");
      break;
    case 3:
      b = random(1, 5);
      a = a * b;
      correctAnswer = a / b;
      lcd.print(String(a) + "/" + String(b) + "=?");
      break;
  }
  
  lcd.setCursor(0, 1);
  lcd.print("Score:" + String(gameScore));
  input = "";
}

void handleGameMode(char key) {
  if (key == 'E') { // ESC
    input = "";
    updateGameDisplay();
  } else if (key == 'N') { // ENT
    checkAnswer();
  } else if (key == '#') {
    if (checkDoublePress()) return;
  } else if (isDigit(key)) {
    input += key;
    updateGameDisplay();
  }
}

void updateGameDisplay() {
  lcd.setCursor(0, 1);
  lcd.print("Ans:" + input + "   ");
}

void checkAnswer() {
  if (input.length() == 0) return;
  
  int answer = input.toInt();
  if (answer == correctAnswer) {
    gameScore++;
    lcd.setCursor(0, 1);
    lcd.print("Correct! :)    ");
    delay(1000);
    generateProblem();
  } else {
    gameAttempts--;
    if (gameAttempts <= 0) {
      gameOver();
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Wrong! Try: " + String(correctAnswer));
      delay(2000);
      generateProblem();
    }
  }
}

void gameOver() {
  lcd.clear();
  delay(50);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Score:" + String(gameScore));
  delay(3000);
  returnToModeSelection();
}

// ============= دوال أساسية مشتركة =============
bool checkDoublePress() {
  static unsigned long lastPress = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastPress < 500) {
    Serial.println("Double press detected, returning to mode selection");
    returnToModeSelection();
    lastPress = 0;
    return true;
  }
  lastPress = currentTime;
  return false;
}

void handleOperator(char op) {
  if (input.length() > 0) {
    result = input.toFloat();
    expression += String(result) + " " + op + " ";
    input = "";
    newInput = true;
    Serial.print("Expression so far: ");
    Serial.println(expression);
    updateDisplay();
  } else {
    Serial.println("No input to operate on!");
  }
}

void appendOperator(char op) {
  if (input.length() > 0) {
    expression += input + " " + op + " ";
    input = "";
    newInput = true;
    Serial.print("Expression so far: ");
    Serial.println(expression);
    updateDisplay();
  } else {
    Serial.println("No input to append operator!");
  }
}

void calculate() {
  if (input.length() > 0) {
    result = input.toFloat();
    expression += String(result);
    input = "";
  }
  if (expression.length() == 0) {
    Serial.println("Nothing to calculate");
    return;
  }

  float tempResult = 0;
  String tempExpr = expression;
  int pos;

  while ((pos = tempExpr.indexOf('^')) != -1) {
    int start = tempExpr.lastIndexOf(' ', pos - 2);
    int end = tempExpr.indexOf(' ', pos + 2);
    if (end == -1) end = tempExpr.length();

    String leftStr = tempExpr.substring(start + 1, pos - 1);
    String rightStr = tempExpr.substring(pos + 2, end);
    float left = leftStr.toFloat();
    float right = rightStr.toFloat();
    tempResult = pow(left, right);

    tempExpr = tempExpr.substring(0, start + 1) + String(tempResult) + tempExpr.substring(end);
    Serial.print("After power: ");
    Serial.println(tempExpr);
  }

  result = 0;
  char op = '+';
  int i = 0;
  while (i < tempExpr.length()) {
    int nextSpace = tempExpr.indexOf(' ', i);
    if (nextSpace == -1) nextSpace = tempExpr.length();
    
    String numStr = tempExpr.substring(i, nextSpace);
    float num = numStr.toFloat();
    
    switch (op) {
      case '+': result += num; break;
      case '-': result -= num; break;
      case '*': result *= num; break;
      case '/':
        if (num != 0) result /= num;
        else {
          showError("Div by Zero");
          return;
        }
        break;
    }
    
    if (nextSpace < tempExpr.length() - 1) {
      op = tempExpr.charAt(nextSpace + 1);
    }
    i = nextSpace + 3;
  }

  Serial.print("Final result: ");
  Serial.println(result);
  
  input = String(result);
  expression = "";
  newInput = true;
  updateDisplay();
}

void evaluateExpression() {
  if (input.length() > 0) {
    expression += input;
    input = "";
  }
  if (expression.length() == 0) {
    Serial.println("Nothing to evaluate");
    return;
  }

  float tempResult = 0;
  String tempExpr = expression;
  int pos;

  while ((pos = tempExpr.indexOf('^')) != -1) {
    int start = tempExpr.lastIndexOf(' ', pos - 2);
    int end = tempExpr.indexOf(' ', pos + 2);
    if (end == -1) end = tempExpr.length();

    String leftStr = tempExpr.substring(start + 1, pos - 1);
    String rightStr = tempExpr.substring(pos + 2, end);
    float left = leftStr.toFloat();
    float right = rightStr.toFloat();
    tempResult = pow(left, right);

    tempExpr = tempExpr.substring(0, start + 1) + String(tempResult) + tempExpr.substring(end);
    Serial.print("After power: ");
    Serial.println(tempExpr);
  }

  result = 0;
  char op = '+';
  int i = 0;
  while (i < tempExpr.length()) {
    int nextSpace = tempExpr.indexOf(' ', i);
    if (nextSpace == -1) nextSpace = tempExpr.length();
    
    String numStr = tempExpr.substring(i, nextSpace);
    float num = numStr.toFloat();
    
    switch (op) {
      case '+': result += num; break;
      case '-': result -= num; break;
      case '*': result *= num; break;
      case '/':
        if (num != 0) result /= num;
        else {
          showError("Div by Zero");
          return;
        }
        break;
    }
    
    if (nextSpace < tempExpr.length() - 1) {
      op = tempExpr.charAt(nextSpace + 1);
    }
    i = nextSpace + 3;
  }

  Serial.print("Final result: ");
  Serial.println(result);
  
  input = String(result);
  expression = "";
  newInput = true;
  updateDisplay();
}

void processInput(char key) {
  if (newInput) {
    input = "";
    newInput = false;
  }
  
  if (isDigit(key) || key == '.') {
    input += key;
  }
  updateDisplay();
}

void clearAll() {
  input = "";
  expression = "";
  result = 0;
  newInput = true;
  powerMode = false;
  updateDisplay();
}

void updateDisplay() {
  lcd.clear();
  delay(50);
  lcd.setCursor(0, 0);
  
  switch (currentMode) {
    case BASIC:
      lcd.print("Basic Mode");
      lcd.setCursor(0, 1);
      if (expression.length() > 0) {
        lcd.print(expression + input);
      } else if (input.length() > 0) {
        lcd.print(input);
      } else {
        lcd.print(result, 4);
      }
      Serial.println("Basic Mode Display Updated");
      break;
    case SCIENTIFIC:
      lcd.print("Scientific Mode");
      lcd.setCursor(0, 1);
      if (expression.length() > 0 || input.length() > 0) {
        lcd.print(expression + input);
      } else {
        lcd.print(result, 4);
      }
      Serial.println("Scientific Mode Display Updated");
      break;
    case UNIT_CONVERTER:
      lcd.print("Unit Converter");
      lcd.setCursor(0, 1);
      switch (currentUnitType) {
        case KG_TO_G: lcd.print("Kg->g:"); break;
        case G_TO_KG: lcd.print("g->Kg:"); break;
        case KM_TO_M: lcd.print("Km->m:"); break;
        case M_TO_KM: lcd.print("m->Km:"); break;
        case C_TO_F: lcd.print("C->F:"); break;
        case F_TO_C: lcd.print("F->C:"); break;
      }
      lcd.print(input);
      Serial.println("Unit Converter Display Updated");
      break;
    case CURRENCY_CONVERTER:
      lcd.print("Currency Conv");
      lcd.setCursor(0, 1);
      switch (currentCurrencyType) {
        case USD_TO_EGP: lcd.print("USD->EGP:"); break;
        case EUR_TO_EGP: lcd.print("EUR->EGP:"); break;
        case GBP_TO_EGP: lcd.print("GBP->EGP:"); break;
        case SAR_TO_EGP: lcd.print("SAR->EGP:"); break;
      }
      lcd.print(input);
      Serial.println("Currency Converter Display Updated");
      break;
    case GAME:
      lcd.print("Game Mode");
      Serial.println("Game Mode Display Updated");
      return;
  }
}

void showError(const char* message) {
  lcd.clear();
  delay(50);
  lcd.print("Error:");
  lcd.setCursor(0, 1);
  lcd.print(message);
  Serial.print("Error Displayed: ");
  Serial.println(message);
  delay(2000);
  updateDisplay();
}

bool isDigit(char c) {
  return c >= '0' && c <= '9';
}