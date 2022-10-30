#ifndef BASICBUTTON_H
#define BASICBUTTON_H

#include <vector>

// General
#define STRING_SIZE 21
#define READ_BUFFER_SIZE 20
#define ACTION_SEPARATOR '-'
#define TOGGLE_MAXTIME 20 * 60 * 1000
//#define TOGGLE_MAXTIME 10 * 1000

// Button actions
#define PRESS "P"
#define DEPRESS "D"

// Button types
enum BtnType {PUSH, TOGGLE};

class Button {
  public:
    char _id[STRING_SIZE];
    char _lastAction[STRING_SIZE];
    long _lastActionTime;
    uint8_t _pin;
    BtnType _type;
    Button(const char* id, uint8_t pin, BtnType type);
    void handle(const char* act);
};

class ButtonHandler {
  public:
    void add(Button* button);
    void update(const char* btnAction);
  private:
    std::vector<Button*> _buttons;
    Button* find(const char* btn);
    void releaseOtherPushButtons(const char* ignoreId);
    void checkTimers();
};

#endif