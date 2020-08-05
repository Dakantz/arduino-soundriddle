#define PIN_PIEZO 3
#define PIN_LED_G 5
#define PIN_LED_R 6
#define PIN_SERVO 9
#include <Servo.h>
int8_t tone_pins[] = {-1, -1, -1, -1, 13, 12, 11, 10, -1};
char names[] = {'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C', 'D'};
int tones[] = {261, 294, 329, 369, 392, 440, 493, 523, 587};
int number_of_tones = 8;
char *sequence = "CCbgag";
int length = 6;
uint8_t next_index = 0;
char current_one = -1;
uint8_t pressed_one = -1;
boolean was_playing = false;
char *success_chime = "ddedgfddedagddDbggfeCCbgag";
uint8_t success_chime_durs[] = {8, 8, 4, 4, 4, 2, 8, 8, 4, 4, 4, 2, 8, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 4, 4, 2};
uint8_t success_chime_length = 26;
char *failure_chime = 'gfe';
uint8_t failure_chime_durs[] = {4, 4, 2};
uint8_t failure_chime_length = 3;
uint16_t tempo = 1200;
boolean was_correct = false;
boolean is_closed = false;
Servo opener;
void moveServoTo(int degrees)
{
    opener.attach(PIN_SERVO);
    opener.write(degrees);
    delay(1500);
    opener.detach();
}
void setup()
{
    Serial.begin(9600);
    Serial.println("Started");
    moveServoTo(90);
    is_closed = true;
}
void playChime(char *chime_names, uint8_t *durs, uint8_t length)
{
    for (int i = 0; i < length; i++)
    {
        int frequency = -1;
        for (int j = 0; j < number_of_tones; j++)
        {
            if (chime_names[i] == names[j])
            {
                frequency = tones[j];
                break;
            }
        }
        uint16_t duration = tempo / durs[i];
        tone(PIN_PIEZO, frequency, duration);
        delay(duration);
    }
}
void loop()
{
    int index = -1;
    for (int i = 0; i < number_of_tones; i++)
    {
        if (tone_pins[i] != -1 && digitalRead(tone_pins[i]))
        {
            index = i;
            Serial.print("Pressed ");
            Serial.println(index);
            break;
        }
    }
    if (index != -1)
    {
        tone(PIN_PIEZO, tones[index], tempo / 4);
    }
    if (was_playing)
    {
        if (index == -1 && was_correct)
        {
            //stopped pressing
            next_index++;
            Serial.print("Got next tone. Now at ");
            Serial.println(next_index);
            if (length == next_index)
            {
                next_index = 0;
                //success
                digitalWrite(PIN_LED_G, 1);
                playChime(success_chime, success_chime_durs, success_chime_length);
                digitalWrite(PIN_LED_G, 0);
                moveServoTo(0);
                is_closed = false;
            }
        }
        else
        {
            //still pressing
            // do nothing
        }
    }
    else
    {
        if (index == -1)
        {
            //does nothing
        }
        else
        {
            //started pressing
            if (sequence[next_index] == names[index])
            {
                was_correct = true;
                // correct
                digitalWrite(PIN_LED_G, 1);
            }
            else
            {

                next_index = 0;
                was_correct = false;
                //incorrect
                digitalWrite(PIN_LED_R, 1);
            }
        }
    }
    if (index != -1)
    {
        was_playing = true;
        delay(tempo / 4);
        digitalWrite(PIN_LED_G, 0);
        digitalWrite(PIN_LED_R, 0);
        if (!is_closed)
        {
            moveServoTo(90);
            is_closed = true;
        }
    }
    else
    {
        was_playing = false;
    }
}
