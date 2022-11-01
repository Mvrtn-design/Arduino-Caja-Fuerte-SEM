//=======================================================
//     Librerías y variables globales del programa
//=======================================================
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Adafruit_Fingerprint.h>    // Libreria  para el Sensor de huella dactilar

SoftwareSerial mySerial(10, 11);     // Crear Serial para Sensor  Rx, TX del Arduino
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


//=============================
//     Configuración LCD
//=============================
LiquidCrystal lcd(32, 30, 28, 26, 24, 22);


//===========================================
//     Configuración teclado matricial
//===========================================
const byte rows = 4;
const byte cols = 4;
int pantalla = 1;
char keys[rows][cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[rows] = {53, 51, 49, 47};//pines asignados, en el arduino están los impares seguidamente
byte colPins[cols] = {45, 43, 41, 39};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

//==============================
//     Variables globales
//==============================

char claveAux[12] = {'#', 5, 1, 5, 2, 5, 3, 5, 4, 5, 5, '#'};
char clave1[4];
char clave2[4];
char introducido[4];
int cont = 0;
int intentos = 3;
int huella = 0;
uint8_t huellaUsuario;


bool existeUsuario = false;
char claveUsuario[4];


//===================================
//=     Prototipos de funciones     =
//===================================

void borrarUsuario();
void fin(int error);//vuelve al inicio, y , si hay un error de teclado ,(1), lo muestra
bool chequeo(int abrir, int cont , int intentos);
void nuevaClave(int ambas);//valdrá 1 si tras hacer esto entrará en nuevaHuella()
void cambiarClave();
bool chequeoHuella(int intentos);
void nuevaHuella();
void cambiarHuella();

//===============================================
//=     Inicio de programa, configuraciones     =
//===============================================
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2); //Cantidad de filas y columnas que en el LCD
  pinMode(6, OUTPUT);

  finger.begin(57600);  // inicializa comunicacion con sensor a 57600 Baudios
  if (!finger.verifyPassword()) {
    Serial.println("No hay comunicacion con el sensor de huella");
    Serial.println("Revise las conexiones");
    while (1) delay(1);
  }

}

//==========================================
//=     Funcionalidad de la aplicación     =
//==========================================
void loop() {
  /*MENÚ:
    A: Abrir Caja
    B: Borrar/Crear Usuario
    C: Cambiar Clave/Huella
  */
  lcd.clear();
  for (int i = 0; i < 3; i++) {
    lcd.setCursor(i, 0);
    lcd.print(">");
    lcd.setCursor(15 - i, 0);
    lcd.print("<");
    delay(600);
  }
  lcd.setCursor(4, 0);
  lcd.print("CAJA SEM");
  delay(1300);
  lcd.setCursor(0, 1);
  lcd.print("elija una opcion");
  char digito = keypad.waitForKey();
  lcd.setCursor(0, 1);
  lcd.print("    op -> ");  lcd.print(digito);  lcd.print("     ");
  
  delay(1400);
  switch (digito) {
    case 'A':
      if (existeUsuario) {
        if (chequeo(0, 0, 3) && chequeoHuella(3))   abrirCaja();
      }
      else  nuevaClave(1);
      break;
    case 'B':
      if (existeUsuario) borrarUsuario();
      else  nuevaClave(1);
      break;
    case 'C':
      if (!existeUsuario) fin(0);
      else {
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("CAMBIO DE:");
        lcd.setCursor(0, 1);
        lcd.print("Clave:1 Huella:0");
        digito = keypad.waitForKey();
        if (digito == '1') {
          lcd.setCursor(0, 1);
          lcd.print("      1          ");
          cambiarClave();
        }
        else if (digito == '0') {
          lcd.setCursor(0, 1);
          lcd.print("      0          ");
          delay(1000);
          cambiarHuella();
        }
        else fin(1);
      }
      break;
    case 'D':

      fin(0);
      break;
    default:
      fin(1);
  }

}
//------------------------------------------------------------
void abrirCaja() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Abriendo caja");
  digitalWrite(6, HIGH);
  delay(9000);
  digitalWrite(6, LOW);
  delay(3000);
  fin(0);
}
//------------------------------------------------------------
void cambiarClave() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CAMBIO DE CLAVE");
  delay(2000);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Ponga primero ");
  delay(1300);
  lcd.setCursor(0, 1);
  lcd.print("la clave antigua");
  delay(2000);
  if (chequeo(0, 0, 3))nuevaClave(0);
}
//------------------------------------------------------------
void cambiarHuella() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CAMBIO DE HUELLA");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ponga primero la");
  delay(1300);
  lcd.setCursor(0, 1);
  lcd.print("antigua huella");
  delay(2000);
  if (chequeoHuella(3)) nuevaHuella();
}
//---------------------------------------------------------
void borrarUsuario() {
  if (existeUsuario) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Borrar Usuario");
    lcd.setCursor(1, 1);
    lcd.print("SI->1  NO->0");
    char opcion = keypad.waitForKey();
    if (opcion == '1') {
      lcd.setCursor(0, 1);
      lcd.print("      1       \n\n");
      delay(1000);
      if (chequeo(0, 0, 3) && chequeoHuella(3)) {
        existeUsuario = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("USUARIO BORRADO");
        fin(0);
      }
      else fin(1);
    }
    else if (opcion == '0') {
      lcd.setCursor(0, 1);
      lcd.print("      0       \n\n");
      fin(0);
    }
    else fin(1);
  }
  else  nuevaClave(0);
}
//----------------------------------------------------------------
bool chequeo(int abrir, int cont , int intentos) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("==== CODIGO ====");
  lcd.setCursor(1, 1);
  for (cont = 0; cont < 4; cont++) {
    introducido[cont] = keypad.waitForKey();
    lcd.print("  *");
  }
  if (cont == 4) {
    abrir = 1;
    delay(500);
    for (int i = 0; i < 4; i++) {
      if (introducido[i] != claveUsuario[i]) {
        abrir = 0;
        cont = 0;
        intentos--;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CODIGO ERRONEO");
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.print("intentos-> ");   lcd.print(intentos);
        delay(1300);
        if (intentos > 0) chequeo(abrir, cont, intentos);
        else fin(1);
      }
    }
    if (abrir == 1) {
      delay(600);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CODIGO CORRECTO");
      delay(1000);
      return true;
    }
  }
  else chequeo(abrir, cont, intentos);
}
//-------------------------------------------------------------
bool chequeoHuella(int intentos) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("==== HUELLA ====");
  while (!getFingerprintEnroll());
  if (finger.fingerFastSearch() != FINGERPRINT_OK) {
    intentos--;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HUELLA ERRONEA");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("intentos-> ");   lcd.print(intentos);
    delay(1300);
    if (intentos > 0) chequeoHuella(intentos);
    else fin(1);
  }
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HUELLA CORRECTA");
    delay(1000);
    return true;
  }
}
//--------------------------------------------------------------
void nuevaClave(int ambas) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NUEVO USUARIO:");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("PONGA UN CODIGO");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("==== CODIGO ====");
  lcd.setCursor(1, 1);
  for (int i = 0; i < 4; i++) {
    clave1[i] = keypad.waitForKey();
    lcd.print("  *");
  }
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("REPITA EL CODIGO");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("==== CODIGO ====");
  lcd.setCursor(1, 1);
  for (int i = 0; i < 4; i++) {
    clave2[i] = keypad.waitForKey();
    lcd.println("  *");
  }
  delay(1000);
  for (int i = 0; i < 4; i++) {
    if (clave1[i] != clave2[i]) {
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("CODIGOS ");
      lcd.setCursor(0, 1);
      lcd.print("NO COINCIDENTES ");
      delay(2000);
      nuevaClave(0);
    }
  }
  for (int i = 0; i < 4; i++) {
    claveUsuario[i] = clave2[i];
  }
  existeUsuario = true;
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("CLAVE");
  lcd.setCursor(2, 1);
  lcd.print("ESTABLECIDA");
  delay(2000);
  if (ambas == 0) fin(0);
  else {
    nuevaHuella();
  }
}
//--------------------------------------------------------------
void nuevaHuella() {
  getFingerprintEnroll();
  guardarHuella();
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NUEVA HUELLA OK");
  fin(0);
}
//--------------------------------------------------------------
void fin(int error) {
  delay(100);
  lcd.clear();
  if (error == 1) {
    lcd.setCursor(3, 0);
    lcd.print("ERROR!!!");
    delay(1000);
  }
  loop();
}
//---------------------------------------------------------------
void guardarHuella() {

  huellaUsuario = finger.storeModel(1);

  if (huellaUsuario != FINGERPRINT_OK){ lcd.setCursor(3, 0);
    lcd.print("ERROR!!!");}
  delay(3000);
}
//------------------------------------------------------------------

uint8_t getFingerprintEnroll() {

  int p = -1;
  int id = 1;
  lcd.setCursor(0, 0);
  lcd.print("Pon la huella");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        //Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        //Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.printfln("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  lcd.clear();


  lcd.println("Otra vez");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        //Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    //Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;

  }

  return true;
}
