#include "basicbutton.h"

#include <string.h>
#include "BluetoothSerial.h"

Button::Button(const char* id, uint8_t pin, BtnType type) {
  strcpy(_id, id);
  strcpy(_lastAction, DEPRESS);
  _lastActionTime = 0l;
  _pin = pin;
  _type = type;

  pinMode(_pin, OUTPUT);
}

void Button::handle(const char* act) {

  boolean isPush = strcmp(act, PRESS) == 0;

  if (_type == PUSH) {
    digitalWrite(_pin, isPush? HIGH : LOW);
    _lastActionTime = millis();
    strcpy(_lastAction, act);
  }
  else if (_type == TOGGLE) { // ignores despress
    if (isPush) {
      if (strcmp(_lastAction, DEPRESS) == 0) {
        digitalWrite(_pin, HIGH);
        strcpy(_lastAction, PRESS);
      }
      else if (strcmp(_lastAction, PRESS) == 0) {
        digitalWrite(_pin, LOW);
        strcpy(_lastAction, DEPRESS);
      }
      _lastActionTime = millis();
    }
  }
}

void ButtonHandler::add(Button* button) {
  _buttons.push_back(button);
}

void ButtonHandler::update(const char* btnAction) {

  if (strcmp(btnAction, "") != 0) {
    //Serial.print("Handling: ");
    //Serial.println(btnAction);

    char btn[20] = "\n";
    char act[20] = "\n";

    char* separatorIdx = strchr(btnAction, ACTION_SEPARATOR);
    char* endIdx = strchr(btnAction, '\n');
    int btnLenght = separatorIdx - btnAction;

    strncpy(btn, btnAction, btnLenght);
    strncpy(act, separatorIdx + 1, READ_BUFFER_SIZE - (btnLenght + 1)); // el + 1 es el ACTION_SEPARATOR
    
    Button* found = find(btn);
    if (found != nullptr) {
      found->handle(act);

      // Si es apretado un pushbutton, soltar los otros pushbuttons
      if (found->_type == PUSH && strcmp(act, PRESS) == 0) {
        releaseOtherPushButtons(found->_id);
      }
    }
  }

  checkTimers();
}

Button* ButtonHandler::find(const char* btn) {
  Button* found = nullptr;
  unsigned int vecSize = _buttons.size();
  for(unsigned int i = 0; i < vecSize; i++)
  {
    if (strcmp(_buttons[i]->_id, btn) == 0) {
      found = _buttons[i];
      break;
    }
  }
  return found;
}

void ButtonHandler::releaseOtherPushButtons(const char* ignoreId) {
  unsigned int vecSize = _buttons.size();
  for(unsigned int i = 0; i < vecSize; i++)
  {
    if (_buttons[i]->_type == PUSH && strcmp(_buttons[i]->_id, ignoreId) != 0) {
      _buttons[i]->handle(DEPRESS);
    }
  }
}

void ButtonHandler::checkTimers() {
  long now = millis();
    unsigned int vecSize = _buttons.size();
  for(unsigned int i = 0; i < vecSize; i++)
  {
    if (_buttons[i]->_type == TOGGLE &&
      strcmp(_buttons[i]->_lastAction, PRESS) == 0 &&
      now - _buttons[i]->_lastActionTime >= TOGGLE_MAXTIME) {
      
      _buttons[i]->handle(PRESS);
    }
  }
}