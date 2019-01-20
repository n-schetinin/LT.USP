#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

#include <IRremote.h>
#include <IRremoteInt.h>


IRsend irsend;
IRrecv irrecv(5);
LiquidCrystal_I2C lcd(EEPROM.read(30), 20, 4);
decode_results results;

const int buttonPin = 7;
const int ledPin =  13;
int perebor;
String temp;
int curBtnState = 0;
int prevBtnState = 0;
long int ammo_signal;
unsigned long int wait, work, kill, kill1, mags, dmg, wmax, wmin;
int anm_val, anm_wait; //значение и время работы аномалии
char mode, mode2, state;
int i;
String setup_conf, setup_vals[4];
char setup_mode, setup_digit;
int setup_line, setup_val;

String conf, tmp, screen;
int len, start;
char convert[1];

unsigned long int time_start, time_end, time_cur, time_prev;
int time_tmp;


bool is_shot(decode_results input) {
  int j;
  char packet[24];
  int packet_len;

  ltoa(input.value, packet, 2);
  packet_len = 0;


  for (j = 0; j < sizeof(packet); j++) {
    if (packet[j] == '0' || packet[j] == '1') {
      packet_len++;
    }
  }


  if (packet_len < 14 || (packet_len < 24 && packet_len > 14)) {
    return true;
  } else {
    return false;
  }
}

void draw_config(String conf, bool writetoee) {
  //EEPROM.write(30,0x3f);
  char eebyte[20];
  Serial.println(conf);
  lcd.clear();
  mode = conf[0];
  if (writetoee) {
    for (i = 0; i < conf.length(); i++) {
      eebyte[i] = conf[i];
    }
    EEPROM.put(0, eebyte);
  }
  if (mode == '1') {
    screen = "Mode:btn  Kill:";
    temp = "";
    for (i = 3; i <= conf.length() ; i++) {
      temp = temp + conf[i - 1];
    }
    kill = atoi(temp.c_str());

    /*screen=screen+temp;*/
    lcd.setCursor(0, 0);
    lcd.print(screen);
    lcd.print(kill);

  } else if (mode == '2' || mode == '3') {

    if (mode == '2') {
      screen = "Mode:time  Work:";
    } else if (mode == '3') {
      screen = "Mode:t+b   Work:";
    }

    tmp = conf[1];
    len = atoi(tmp.c_str());
    temp = "";
    for (i = 2; i <= 1 + len; i++) {
      temp = temp + conf[i];
    }
    work = atoi(temp.c_str());
    lcd.setCursor(0, 0);
    lcd.print(screen);
    lcd.print(work);

    screen = "Wait:";

    start = 2 + len;
    tmp = conf[start];
    len = atoi(tmp.c_str());
    temp = "";
    for (i = start + 1; i <= start + len; i++) {
      temp = temp + conf[i];
    }
    wait = atoi(temp.c_str());

    screen = screen + temp;
    lcd.setCursor(0, 1);
    lcd.print(screen);
    screen = "Kill:";

    start = start + len + 1;
    tmp = conf[start];
    len = atoi(tmp.c_str());
    temp = "";
    for (i = start + 1; i <= start + len; i++) {
      temp = temp + conf[i];
    }
    kill = atoi(temp.c_str());
    screen = screen + temp;
    lcd.setCursor(11, 1);
    lcd.print(screen);
  } else if (mode == '7') {


    screen = "Mode:rad   Dmg :";

    tmp = conf[1];
    len = atoi(tmp.c_str());
    temp = "";
    for (i = 2; i <= 1 + len; i++) {
      temp = temp + conf[i];
    }
    dmg = atoi(temp.c_str());
    lcd.setCursor(0, 0);
    lcd.print(screen);
    lcd.print(dmg);

    screen = "Wmin:";

    start = 2 + len;
    tmp = conf[start];
    len = atoi(tmp.c_str());
    temp = "";
    for (i = start + 1; i <= start + len; i++) {
      temp = temp + conf[i];
    }
    wmin = atoi(temp.c_str());

    screen = screen + temp;
    lcd.setCursor(0, 1);
    lcd.print(screen);
    screen = "Wmax:";

    start = start + len + 1;
    tmp = conf[start];
    len = atoi(tmp.c_str());
    temp = "";
    for (i = start + 1; i <= start + len; i++) {
      temp = temp + conf[i];
    }
    wmax = atoi(temp.c_str());
    screen = screen + temp;
    lcd.setCursor(11, 1);
    lcd.print(screen);
  } else if (mode == '4') {
    lcd.setCursor(0, 0);
    lcd.print("Mode: shot");
  } else if (mode == '5') {
    screen = "Mode:ammo  Mags:";
    temp = conf[1];
    mags = atoi(temp.c_str());
    switch (mags) {
      case 1:
        ammo_signal = 0x8A01E8;
        break;
      case 2:
        ammo_signal = 0x8A02E8;
        break;
      case 3:
        ammo_signal = 0x8A03E8;
        break;
      case 4:
        ammo_signal = 0x8A04E8;
        break;
      case 5:
        ammo_signal = 0x8A05E8;
        break;
    }

    lcd.setCursor(0, 0);
    lcd.print(screen);
    lcd.print(mags);

    screen = "Wait:";

    len = 0;
    start = 2 + len;
    tmp = conf[start];
    len = atoi(tmp.c_str());
    temp = "";
    for (i = start + 1; i <= start + len; i++) {
      temp = temp + conf[i];
    }
    wait = atoi(temp.c_str());

    screen = screen + temp;
    lcd.setCursor(0, 1);
    lcd.print(screen);
    screen = "Kill:";

    start = start + len + 1;
    tmp = conf[start];
    len = atoi(tmp.c_str());
    temp = "";
    for (i = start + 1; i <= start + len; i++) {
      temp = temp + conf[i];
    }
    kill = atoi(temp.c_str());
    screen = screen + temp;
    lcd.setCursor(11, 1);
    lcd.print(screen);
  } else if (mode == '6') {
    screen = "Mode:armor";

    lcd.setCursor(0, 0);
    lcd.print(screen);

    screen = "Wait:";

    len = 0;
    start = 1 + len;
    tmp = conf[start];
    len = atoi(tmp.c_str());
    temp = "";
    for (i = start + 1; i <= start + len; i++) {
      temp = temp + conf[i];
    }
    wait = atoi(temp.c_str());

    screen = screen + temp;
    lcd.setCursor(0, 1);
    lcd.print(screen);
    screen = "Kill:";

    start = start + len + 1;
    tmp = conf[start];
    len = atoi(tmp.c_str());
    temp = "";
    for (i = start + 1; i <= start + len; i++) {
      temp = temp + conf[i];
    }
    kill = atoi(temp.c_str());
    screen = screen + temp;
    lcd.setCursor(11, 1);
    lcd.print(screen);
  }

  lcd.setCursor(0, 2);
  lcd.print(" Remote OK to setup");
  lcd.setCursor(0, 3);
  lcd.print("  Button to start");

}

void setup() {
  char eebyte[20];
  //EEPROM.write(30, 0x3f);

  int y;
  lcd.init();
  lcd.backlight();

  conf = "";

  EEPROM.get(0, eebyte);
  for (y = 0; y < 20; y++) {
    conf = conf + eebyte[y];
  }

  draw_config(conf, false);

  irrecv.enableIRIn();
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  state = '0';

  lcd.setCursor(0, 2);
  lcd.print(" Remote OK to setup");
  lcd.setCursor(0, 3);
  lcd.print("  Button to start");
}

void loop() {
  if (state == '2') {
    if (irrecv.decode(&results)) {
      if (setup_val == 1) {
        switch (results.value) {
          case 16753245:
            setup_mode = '1';
            setup_line++;
            lcd.print(" btn");
            lcd.setCursor(6, setup_line);
            lcd.print("Kill: ");
            setup_val = 4;
            break;
          case 16736925:
            setup_mode = '2';
            setup_line++;
            lcd.print("time");
            lcd.setCursor(6, setup_line);
            lcd.print("Work: ");
            setup_val = 2;
            break;
          case 16769565:
            setup_mode = '3';
            setup_line++;
            lcd.print(" t+b");
            lcd.setCursor(6, setup_line);
            lcd.print("Work: ");
            setup_val = 2;
            break;
          case 16720605:
            setup_mode = '4';
            lcd.print("shot");
            state = '0';
            setup_conf = "4";
            draw_config(setup_conf, true);
            break;
          case 16712445:
            setup_mode = '5';
            setup_line++;
            lcd.print("ammo");
            lcd.setCursor(6, setup_line);
            lcd.print("Mags: ");
            setup_val = 5;
            break;
          case 16761405:
            setup_mode = '6';
            setup_line++;
            lcd.print("armor");
            lcd.setCursor(6, setup_line);
            lcd.print("Wait: ");
            setup_val = 3;
            break;
          case 16769055:
            setup_mode = '7';
            setup_line++;
            lcd.print("rad");
            lcd.setCursor(6, setup_line);
            lcd.print("Dmg : ");
            setup_val = 6;
            break;
        }
      } else if (setup_val == 2 || setup_val == 3 || setup_val == 4) {

        setup_digit = '\0';
        switch (results.value) {
          case 16753245:
            setup_digit = '1';
            break;
          case 16736925:
            setup_digit = '2';
            break;
          case 16769565:
            setup_digit = '3';
            break;
          case 16720605:
            setup_digit = '4';
            break;
          case 16712445:
            setup_digit = '5';
            break;
          case 16761405:
            setup_digit = '6';
            break;
          case 16769055:
            setup_digit = '7';
            break;
          case 16754775:
            setup_digit = '8';
            break;
          case 16748655:
            setup_digit = '9';
            break;
          case 16750695:
            setup_digit = '0';
            break;
          case 16738455:
            setup_digit = '*';
            break;
          case 16756815:
            setup_digit = '#';
            break;
          case 16726215:
            setup_digit = '!';
            break;
        }

        if (setup_digit != '\0' && setup_digit != '#' && setup_digit != '*' && setup_digit != '!') {
          setup_vals[setup_val - 2] = setup_vals[setup_val - 2] + setup_digit;
        } else if (setup_digit == '*' || setup_digit == '#') {
          setup_vals[setup_val - 2] = "";
        }
        lcd.setCursor(12, setup_line);
        lcd.print(setup_vals[setup_val - 2]);
        lcd.print("    ");

        if (setup_vals[setup_val - 2].length() == 4 || (setup_digit == '!' && setup_vals[setup_val - 2].length() > 0) || (setup_digit == '0' && setup_vals[setup_val - 2].length() == 1)) {
          if (setup_val < 4) {
            setup_val++;
            setup_line++;
            if (setup_val == 3) {
              lcd.setCursor(6, setup_line);
              lcd.print("Wait: ");
            } else if (setup_val == 4) {
              lcd.setCursor(6, setup_line);
              lcd.print("Kill: ");
            }
          } else {
            setup_conf = setup_mode;
            if (setup_mode == '5') {
              setup_conf = setup_conf + setup_vals[3];
            }
            for (i = 0; i <= 2; i++) {
              if (setup_vals[i].length() != 0) {
                itoa(setup_vals[i].length(), convert, 10);
                setup_conf = setup_conf + convert;
                setup_conf = setup_conf + setup_vals[i];
              }
            }
            draw_config(setup_conf, true);
            //Serial.println(setup_conf);
            state = '0';
          }
        }
      } else if (setup_val == 5) {
        switch (results.value) {
          case 16753245:
            setup_vals[3] = '1';
            setup_line++;
            break;
          case 16736925:
            setup_vals[3] = '2';
            setup_line++;
            break;
          case 16769565:
            setup_vals[3] = '3';
            setup_line++;
            break;
          case 16720605:
            setup_vals[3] = '4';
            setup_line++;
            break;
          case 16712445:
            setup_vals[3] = '5';
            setup_line++;
            break;
        }
        lcd.print(setup_vals[3]);
        lcd.setCursor(6, setup_line);
        lcd.print("Wait: ");
        setup_val = 3;
      } else if (setup_val == 6 || setup_val == 7 || setup_val == 8) {

        setup_digit = '\0';
        switch (results.value) {
          case 16753245:
            setup_digit = '1';
            break;
          case 16736925:
            setup_digit = '2';
            break;
          case 16769565:
            setup_digit = '3';
            break;
          case 16720605:
            setup_digit = '4';
            break;
          case 16712445:
            setup_digit = '5';
            break;
          case 16761405:
            setup_digit = '6';
            break;
          case 16769055:
            setup_digit = '7';
            break;
          case 16754775:
            setup_digit = '8';
            break;
          case 16748655:
            setup_digit = '9';
            break;
          case 16750695:
            setup_digit = '0';
            break;
          case 16738455:
            setup_digit = '*';
            break;
          case 16756815:
            setup_digit = '#';
            break;
          case 16726215:
            setup_digit = '!';
            break;
        }

        if (setup_digit != '\0' && setup_digit != '#' && setup_digit != '*' && setup_digit != '!') {
          setup_vals[setup_val - 6] = setup_vals[setup_val - 6] + setup_digit;
        } else if (setup_digit == '*' || setup_digit == '#') {
          setup_vals[setup_val - 6] = "";
        }
        lcd.setCursor(12, setup_line);
        lcd.print(setup_vals[setup_val - 6]);
        lcd.print("    ");

        if (setup_vals[setup_val - 6].length() == 3 || (setup_digit == '!' && setup_vals[setup_val - 6].length() > 0) || (setup_digit == '0' && setup_vals[setup_val - 6].length() == 1)) {
          if (setup_val < 8) {
            setup_val++;
            setup_line++;
            if (setup_val == 7) {
              lcd.setCursor(6, setup_line);
              lcd.print("Wmin: ");
            } else if (setup_val == 8) {
              lcd.setCursor(6, setup_line);
              lcd.print("Wmax: ");
            }
          } else {
            setup_conf = setup_mode;
            if (setup_mode == '5') {
              setup_conf = setup_conf + setup_vals[3];
            }
            for (i = 0; i <= 2; i++) {
              if (setup_vals[i].length() != 0) {
                itoa(setup_vals[i].length(), convert, 10);
                setup_conf = setup_conf + convert;
                setup_conf = setup_conf + setup_vals[i];
              }
            }
            draw_config(setup_conf, true);
            //Serial.println(setup_conf);
            state = '0';
          }
        }
      }
      irrecv.resume();
    }

  } else if (state == '0') {

    if (irrecv.decode(&results) && results.value == 16726215) {
      setup_val = 1;
      state = '2';
      setup_conf = "";
      setup_vals[0] = "";
      setup_vals[1] = "";
      setup_vals[2] = "";
      setup_line = 0;
      lcd.clear();
      lcd.setCursor(0, setup_line);
      lcd.print("Setup Mode: ");
      irrecv.resume();
    }
    curBtnState = digitalRead(buttonPin);
    if (curBtnState != prevBtnState ) {
      if (curBtnState == HIGH) {
        state = '1';
        if (mode == '1') {
          lcd.clear();
          lcd.setCursor(7, 0);
          lcd.print("Medic");
          lcd.setCursor(0, 2);
          lcd.print("    Press button");
          lcd.setCursor(0, 3);
          lcd.print("      to heal");
          mode2 = '1';
        } else if (mode == '4') {
          lcd.clear();
          lcd.setCursor(7, 0);
          lcd.print("Medic");
          lcd.setCursor(0, 2);
          lcd.print("    Shot to heal");
        } else if (mode == '3' || mode == '2') {
          lcd.clear();
          lcd.setCursor(7, 0);
          lcd.print("Medic");
          lcd.setCursor(0, 2);
          lcd.print("      Healing");
          mode2 = '1';
          time_start = millis();
          time_prev = time_start;
          time_end = time_start + (work * 1000);
          digitalWrite(ledPin, HIGH);
          irsend.sendSony(0x8305E8, 24);
          digitalWrite(ledPin, LOW);
          irrecv.enableIRIn();
        } else if (mode == '5') {
          lcd.clear();
          lcd.setCursor(7, 0);
          lcd.print("Ammo");
          lcd.setCursor(0, 2);
          lcd.print("       Wait");
          mode2 = '2';
          time_start = millis();
          time_prev = time_start;
          time_end = time_start + (wait * 1000);
          digitalWrite(ledPin, HIGH);
          irsend.sendSony(ammo_signal, 24);
          digitalWrite(ledPin, LOW);
          irrecv.enableIRIn();
        } else if (mode == '6') {
          lcd.clear();
          lcd.setCursor(7, 0);
          lcd.print("Armor");
          lcd.setCursor(0, 2);
          lcd.print("       Wait");
          mode2 = '2';
          time_start = millis();
          time_prev = time_start;
          time_end = time_start + (wait * 1000);
          digitalWrite(ledPin, HIGH);
          irsend.sendSony(0x8322E8, 24);
          digitalWrite(ledPin, LOW);
          irrecv.enableIRIn();
        } else if (mode == '7') {
          lcd.clear();
          lcd.setCursor(6, 1);
          lcd.print("!DANGER!");
          lcd.setCursor(4, 2);
          lcd.print("!RADIATION!");
          mode2 = '2';      
        }
      }
      prevBtnState = curBtnState;
    }

  } else if (state == '1') {
    if (mode == '1') {
      if (mode2 == '1') {
        curBtnState = digitalRead(buttonPin);
        if (curBtnState != prevBtnState ) {
          if (curBtnState == HIGH) {
            digitalWrite(ledPin, HIGH);
            irsend.sendSony(0x8305E8, 24);
            irrecv.enableIRIn();
            digitalWrite(ledPin, LOW);
          }
          prevBtnState = curBtnState;
        }
        if (irrecv.decode(&results)) {
          if (is_shot(results) && kill != 0) {
            mode2 = '2';
            lcd.clear();
            lcd.setCursor(7, 0);
            lcd.print("Medic");
            lcd.setCursor(0, 2);
            lcd.print("     Destroyed!");
            time_start = millis();
            time_end = time_start + (kill * 1000);
            time_cur = ((kill * 1000) - (millis() - time_start)) / 1000;
            time_tmp = time_cur / 60;
            lcd.setCursor(6, 3);
            if (time_tmp >= 100) {
              lcd.print(time_tmp);
              lcd.print(":");
            } else if (time_tmp < 100 && time_tmp >= 10) {
              lcd.print(" ");
              lcd.print(time_tmp);
              lcd.print(":");
            } else {
              lcd.print(" 0");
              lcd.print(time_tmp);
              lcd.print(":");
            }
            time_tmp = time_cur % 60;
            if (time_tmp >= 10) {
              lcd.print(time_tmp);
            } else {
              lcd.print("0");
              lcd.print(time_tmp);
            }
          }
          irrecv.resume();
        }
      } else if (mode2 == '2') {
        if (millis() < time_end) {
          time_cur = ((kill * 1000) - (millis() - time_start)) / 1000;
          time_tmp = time_cur / 60;
          lcd.setCursor(6, 3);
          if (time_tmp >= 100) {
            lcd.print(time_tmp);
            lcd.print(":");
          } else if (time_tmp < 100 && time_tmp >= 10) {
            lcd.print(" ");
            lcd.print(time_tmp);
            lcd.print(":");
          } else {
            lcd.print(" 0");
            lcd.print(time_tmp);
            lcd.print(":");
          }
          time_tmp = time_cur % 60;
          if (time_tmp >= 10) {
            lcd.print(time_tmp);
          } else {
            lcd.print("0");
            lcd.print(time_tmp);
          }
        } else {
          mode2 = '1';
          lcd.clear();
          lcd.setCursor(7, 0);
          lcd.print("Medic");
          lcd.setCursor(0, 2);
          lcd.print("    Press button");
          lcd.setCursor(0, 3);
          lcd.print("      to heal");
        }
      }


    } else if (mode == '4') {
      if (irrecv.decode(&results)) {

        if (is_shot(results)) {
          digitalWrite(ledPin, HIGH);
          delay(500);
          irsend.sendSony(0x8305E8, 24);

          digitalWrite(ledPin, LOW);
        }
        irrecv.enableIRIn();
        irrecv.resume();
      }
    } else if (mode == '7') {
      anm_val = random(1, dmg+1) * 256;
      anm_wait = random(wmin, wmax + 1);
      delay(anm_wait * 1000);
      digitalWrite(ledPin, HIGH);
      irsend.sendSony(0xA000E8+anm_val, 24);
      //irsend.sendSony(0xA003E8, 24);
      
      digitalWrite(ledPin, LOW);
      irrecv.enableIRIn();
      
    } else if (mode == '2' || mode == '3') {
      if (mode2 == '1' || mode2 == '2' || mode2 == '0') {
        if (irrecv.decode(&results)) {
          if (is_shot(results) && kill != 0) {
            lcd.clear();
            lcd.setCursor(7, 0);
            lcd.print("Medic");
            lcd.setCursor(0, 2);
            lcd.print("     Destroyed!");
            time_start = millis();
            if (mode2 == '1' || mode2 == '0') {
              kill1 = kill + wait;
            } else if (mode2 == '2') {
              kill1 = kill + time_cur ;
            }
            time_end = time_start + (kill1 * 1000);
            time_cur = ((kill1 * 1000) - (millis() - time_start)) / 1000;
            mode2 = '3';
          }
          irrecv.resume();
        }
      }
      if (mode2 == '1') {
        if (millis() < time_end) {
          if (millis() - time_prev > 1000) {
            time_prev = millis();
            digitalWrite(ledPin, HIGH);
            irsend.sendSony(0x8305E8, 24);
            digitalWrite(ledPin, LOW);
            irrecv.enableIRIn();
          }
          time_cur = ((work * 1000) - (millis() - time_start)) / 1000;
          time_tmp = time_cur / 60;
          lcd.setCursor(6, 3);
          if (time_tmp >= 100) {
            lcd.print(time_tmp);
            lcd.print(":");
          } else if (time_tmp < 100 && time_tmp >= 10) {
            lcd.print(" ");
            lcd.print(time_tmp);
            lcd.print(":");
          } else {
            lcd.print(" 0");
            lcd.print(time_tmp);
            lcd.print(":");
          }
          time_tmp = time_cur % 60;
          if (time_tmp >= 10) {
            lcd.print(time_tmp);
          } else {
            lcd.print("0");
            lcd.print(time_tmp);
          }
        } else {
          mode2 = '2';
          lcd.clear();
          lcd.setCursor(7, 0);
          lcd.print("Medic");
          lcd.setCursor(0, 2);
          lcd.print("       Wait");
          time_start = millis();
          time_prev = time_start;
          time_end = time_start + (wait * 1000);
        }
      } else if (mode2 == '2') {
        if (millis() < time_end) {
          time_cur = ((wait * 1000) - (millis() - time_start)) / 1000;
          time_tmp = time_cur / 60;
          lcd.setCursor(6, 3);
          if (time_tmp >= 100) {
            lcd.print(time_tmp);
            lcd.print(":");
          } else if (time_tmp < 100 && time_tmp >= 10) {
            lcd.print(" ");
            lcd.print(time_tmp);
            lcd.print(":");
          } else {
            lcd.print(" 0");
            lcd.print(time_tmp);
            lcd.print(":");
          }
          time_tmp = time_cur % 60;
          if (time_tmp >= 10) {
            lcd.print(time_tmp);
          } else {
            lcd.print("0");
            lcd.print(time_tmp);
          }
        } else {
          if (mode == '2') {
            lcd.clear();
            lcd.setCursor(7, 0);
            lcd.print("Medic");
            lcd.setCursor(0, 2);
            lcd.print("      Healing");
            mode2 = '1';
            time_start = millis();
            time_prev = time_start;
            time_end = time_start + (work * 1000);
            digitalWrite(ledPin, HIGH);
            irsend.sendSony(0x8305E8, 24);
            digitalWrite(ledPin, LOW);
            irrecv.enableIRIn();
          } else if (mode == '3') {
            mode2 = '0';
            prevBtnState = digitalRead(buttonPin);
            lcd.clear();
            lcd.setCursor(7, 0);
            lcd.print("Medic");
            lcd.setCursor(0, 2);
            lcd.print("       Ready");
            lcd.setCursor(0, 3);
            lcd.print("   Button to heal");
          }
        }
      } else if (mode2 == '0') {
        curBtnState = digitalRead(buttonPin);
        if (curBtnState != prevBtnState ) {
          if (curBtnState == HIGH) {
            lcd.clear();
            lcd.setCursor(7, 0);
            lcd.print("Medic");
            lcd.setCursor(0, 2);
            lcd.print("      Healing");
            mode2 = '1';
            time_start = millis();
            time_prev = time_start;
            time_end = time_start + (work * 1000);
            digitalWrite(ledPin, HIGH);
            irsend.sendSony(0x8305E8, 24);
            digitalWrite(ledPin, LOW);
            irrecv.enableIRIn();
          }
          prevBtnState = curBtnState;
        }
      } else if (mode2 == '3') {
        if (millis() < time_end) {
          time_cur = ((kill1 * 1000) - (millis() - time_start)) / 1000;
          time_tmp = time_cur / 60;
          lcd.setCursor(6, 3);
          if (time_tmp >= 100) {
            lcd.print(time_tmp);
            lcd.print(":");
          } else if (time_tmp < 100 && time_tmp >= 10) {
            lcd.print(" ");
            lcd.print(time_tmp);
            lcd.print(":");
          } else {
            lcd.print(" 0");
            lcd.print(time_tmp);
            lcd.print(":");
          }
          time_tmp = time_cur % 60;
          if (time_tmp >= 10) {
            lcd.print(time_tmp);
          } else {
            lcd.print("0");
            lcd.print(time_tmp);
          }
        } else {
          if (mode == '2') {
            lcd.clear();
            lcd.setCursor(7, 0);
            lcd.print("Medic");
            lcd.setCursor(0, 2);
            lcd.print("      Healing");
            mode2 = '1';
            time_start = millis();
            time_prev = time_start;
            time_end = time_start + (work * 1000);
            digitalWrite(ledPin, HIGH);
            irsend.sendSony(0x8305E8, 24);
            digitalWrite(ledPin, LOW);
            irrecv.enableIRIn();
          } else if (mode == '3') {
            mode2 = '0';
            prevBtnState = digitalRead(buttonPin);
            lcd.clear();
            lcd.setCursor(7, 0);
            lcd.print("Medic");
            lcd.setCursor(0, 2);
            lcd.print("       Ready");
            lcd.setCursor(0, 3);
            lcd.print("   Button to heal");
          }
        }
      }
    } else if (mode == '5') {
      if (mode2 == '1' || mode2 == '2' || mode2 == '0') {
        if (irrecv.decode(&results)) {
          if (is_shot(results) && kill != 0) {
            lcd.clear();
            lcd.setCursor(7, 0);
            lcd.print("Ammo");
            lcd.setCursor(0, 2);
            lcd.print("     Destroyed!");
            time_start = millis();
            kill1 = kill + time_cur ;
            time_end = time_start + (kill1 * 1000);
            time_cur = ((kill1 * 1000) - (millis() - time_start)) / 1000;
            mode2 = '3';
          }
          irrecv.resume();
        }
      }
      if (mode2 == '2') {
        if (millis() < time_end) {
          time_cur = ((wait * 1000) - (millis() - time_start)) / 1000;
          time_tmp = time_cur / 60;
          lcd.setCursor(6, 3);
          if (time_tmp >= 100) {
            lcd.print(time_tmp);
            lcd.print(":");
          } else if (time_tmp < 100 && time_tmp >= 10) {
            lcd.print(" ");
            lcd.print(time_tmp);
            lcd.print(":");
          } else {
            lcd.print(" 0");
            lcd.print(time_tmp);
            lcd.print(":");
          }
          time_tmp = time_cur % 60;
          if (time_tmp >= 10) {
            lcd.print(time_tmp);
          } else {
            lcd.print("0");
            lcd.print(time_tmp);
          }
        } else {
          time_start = millis();
          time_prev = time_start;
          time_end = time_start + (wait * 1000);
          digitalWrite(ledPin, HIGH);
          irsend.sendSony(ammo_signal, 24);
          digitalWrite(ledPin, LOW);
          irrecv.enableIRIn();
        }
      } else if (mode2 == '3') {
        if (millis() < time_end) {
          time_cur = ((kill1 * 1000) - (millis() - time_start)) / 1000;
          time_tmp = time_cur / 60;
          lcd.setCursor(6, 3);
          if (time_tmp >= 100) {
            lcd.print(time_tmp);
            lcd.print(":");
          } else if (time_tmp < 100 && time_tmp >= 10) {
            lcd.print(" ");
            lcd.print(time_tmp);
            lcd.print(":");
          } else {
            lcd.print(" 0");
            lcd.print(time_tmp);
            lcd.print(":");
          }
          time_tmp = time_cur % 60;
          if (time_tmp >= 10) {
            lcd.print(time_tmp);
          } else {
            lcd.print("0");
            lcd.print(time_tmp);
          }
        } else {
          lcd.clear();
          lcd.setCursor(7, 0);
          lcd.print("Ammo");
          lcd.setCursor(0, 2);
          lcd.print("       Wait");
          mode2 = '2';
          time_start = millis();
          time_prev = time_start;
          time_end = time_start + (wait * 1000);
          digitalWrite(ledPin, HIGH);
          irsend.sendSony(ammo_signal, 24);
          digitalWrite(ledPin, LOW);
          irrecv.enableIRIn();
        }

      }
    } else if (mode == '6') {
      if (mode2 == '1' || mode2 == '2' || mode2 == '0') {
        if (irrecv.decode(&results)) {
          if (is_shot(results) && kill != 0) {
            lcd.clear();
            lcd.setCursor(7, 0);
            lcd.print("Armor");
            lcd.setCursor(0, 2);
            lcd.print("     Destroyed!");
            time_start = millis();
            kill1 = kill + time_cur ;
            time_end = time_start + (kill1 * 1000);
            time_cur = ((kill1 * 1000) - (millis() - time_start)) / 1000;
            mode2 = '3';
          }
          irrecv.resume();
        }
      }
      if (mode2 == '2') {
        if (millis() < time_end) {
          time_cur = ((wait * 1000) - (millis() - time_start)) / 1000;
          time_tmp = time_cur / 60;
          lcd.setCursor(6, 3);
          if (time_tmp >= 100) {
            lcd.print(time_tmp);
            lcd.print(":");
          } else if (time_tmp < 100 && time_tmp >= 10) {
            lcd.print(" ");
            lcd.print(time_tmp);
            lcd.print(":");
          } else {
            lcd.print(" 0");
            lcd.print(time_tmp);
            lcd.print(":");
          }
          time_tmp = time_cur % 60;
          if (time_tmp >= 10) {
            lcd.print(time_tmp);
          } else {
            lcd.print("0");
            lcd.print(time_tmp);
          }
        } else {
          time_start = millis();
          time_prev = time_start;
          time_end = time_start + (wait * 1000);
          digitalWrite(ledPin, HIGH);
          irsend.sendSony(0x8322E8, 24);
          digitalWrite(ledPin, LOW);
          irrecv.enableIRIn();
        }
      } else if (mode2 == '3') {
        if (millis() < time_end) {
          time_cur = ((kill1 * 1000) - (millis() - time_start)) / 1000;
          time_tmp = time_cur / 60;
          lcd.setCursor(6, 3);
          if (time_tmp >= 100) {
            lcd.print(time_tmp);
            lcd.print(":");
          } else if (time_tmp < 100 && time_tmp >= 10) {
            lcd.print(" ");
            lcd.print(time_tmp);
            lcd.print(":");
          } else {
            lcd.print(" 0");
            lcd.print(time_tmp);
            lcd.print(":");
          }
          time_tmp = time_cur % 60;
          if (time_tmp >= 10) {
            lcd.print(time_tmp);
          } else {
            lcd.print("0");
            lcd.print(time_tmp);
          }
        } else {
          lcd.clear();
          lcd.setCursor(7, 0);
          lcd.print("Armor");
          lcd.setCursor(0, 2);
          lcd.print("       Wait");
          mode2 = '2';
          time_start = millis();
          time_prev = time_start;
          time_end = time_start + (wait * 1000);
          digitalWrite(ledPin, HIGH);
          irsend.sendSony(0x8322E8, 24);
          digitalWrite(ledPin, LOW);
          irrecv.enableIRIn();
        }

      }
    }
  }
  /*
    16753245 - 1
    16736925 - 2
    16769565 - 3
    16720605 - 4
    16712445 - 5
    16761405 - 6
    16769055 - 7
    16754775 - 8
    16748655 - 9
    16750695 - 0
    16738455 - *
    16756815 - #
  */


}
