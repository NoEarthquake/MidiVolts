/*
  MidiVolts - Sample code for public use by Space Brain Circuits... mono working
*/

// ************************************* USER DEFINE PARAMETERS *************************************

// PLEASE CHOOSE MODE
// 1 VOICE MONOPHONIC ON V0 WITH VELOCITY ON V1, AFTERTOUCH ON V2, AND MOD WHEEL ON V3: ............1
// 2 VOICE DUOPHONIC ON V0, V1 WITH VELOCITY ON V2 AND V3 RESPECTIVELY: ............................2
// 3 VOICE POLYPHONIC ON V0, V1, V2 WITH MOD WHEEL ON V3: ..........................................3
// 4 VOICE POLYPHONIC ON V0, V1, V2, V3: ...........................................................4
// 4 VOICE UNISON ON V0, V1, V2, V3: ...............................................................5

#define MODE 4

// CHOOSE RANGE OF SEMITONES FOR PITCH BEND WHEEL (DEFAULT UP: 2, DEFAULT DOWN: 12)

#define PITCH_BEND_SEMITONES_UP 2
#define PITCH_BEND_SEMITONES_DOWN 12

// CHOOSE CONTROL FUNCTION DURING CC STATUS BYTE 192 (DEFAULT 1: MOD WHEEL)

#define CONTROL_FUNCTION 1

// CHOOSE STATUS BYTE WHEN READING AFTERTOUCH (DEFAULT 160: POLYPHONIC AFTERTOUCH)
#define AFTERTOUCH 160

// CHOOSE PIN TO OUTPUT CLOCK PULSE (DEFAULT: 8)
#define CLOCK_PIN 8

// PIN NUMBER ASSIGNMENTS
#define V0 0
#define V1 1
#define V2 2
#define V3 3

// **************************************************************************************************

#include "MidiVolts.h"
#include "Midi.h"

Midi midi; //Initialize class for listening to Midi Messages
uint8_t UnusedNotes[8]; //Used to track held notes
uint8_t voices; //Number of voices set per MODE selection

MidiVolts voice[4] = {MidiVolts(10, V0), MidiVolts(11, V1), MidiVolts(12, V2), MidiVolts(13, V3)}; // MidiVolts(GatePin, Channel)

 int cvInputs = 16; // how many analog inputs are used
  int cvLevels[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int analogInputs[16] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15}; // analog inputs
  int midiChannel = 0; // MIDI channels 0-15 are 1-16
  int midiCCnumbers[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; // define CC# for each CV input
  int midiCCvalues[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup() {

  // for fine tune calibration, please see README or Calibration Guide
  // sample code for calibration
  //  voice[0].Gain = 0.998;
  //  voice[0].Offset = 0;
  //  voice[1].Gain = 1.;
  //  voice[1].Offset = 0;
  //  voice[2].Gain = 0.994;
  //  voice[2].Offset = 0;
  //  voice[3].Gain = 1.001;
  //  voice[3].Offset = 0.01;

  Serial.begin(31250); //Midi baud
  Wire.begin();

  pinMode(CLOCK_PIN, OUTPUT);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);
  pinMode(A14, INPUT);
  pinMode(A15, INPUT);

//  int cvInputs = 16; // how many analog inputs are used
//  int cvLevels[cvInputs] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//  int analogInputs[cvInputs] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15}; // analog inputs
//  int midiChannel = 0; // MIDI channels 0-15 are 1-16
//  int midiCCnumbers[cvInputs] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; // define CC# for each CV input
//  int midiCCvalues[cvInputs] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  memset(UnusedNotes, 128, sizeof(UnusedNotes));

  if (MODE == 1) {
    voice[V0].VelocityPin = V1; //pitch CV set on V0 will have corresponding Velocity on V1
  }
  else if (MODE == 2)
  {
    voice[V0].VelocityPin = V2; //pitch CV set on V0 will have corresponding Velocity on V2
    voice[V1].VelocityPin = V3; //pitch CV set on V1 will have corresponding Velocity on V3
  }

  if (MODE == 5)
  {
    voices = 1;
  }
  else
  {
    voices = MODE;
  }
}

void loop() {

  midi.Listen(); // Listen for Midi Messages

  if (midi.On == true)
  {
    NoteOn(midi.MidiNum, midi.Velocity); // Set Voltage to Voice, Velocity and turn on Gate
  }

  if (midi.Off == true)
  {
    NoteOff(midi.MidiNum); // Turn off Gate
  }

  if (midi.BendOn == true) // Bend notes
  {
    if (MODE == 1)
    {
      voice[V0].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
    }
    else if (MODE == 2)
    {
      voice[V0].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
      voice[V1].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
    }
    else if (MODE == 3)
    {
      voice[V0].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
      voice[V1].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
      voice[V2].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
    }
    else // MODE 4 or 5
    {
      voice[V0].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
      voice[V1].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
      voice[V2].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
      voice[V3].Bend(midi.Bend, PITCH_BEND_SEMITONES_UP, PITCH_BEND_SEMITONES_DOWN);
    }
  }

  if (midi.ControlOn == true) // CC
  {
    if (midi.ControlFunction == CONTROL_FUNCTION)
    {
      if (MODE == 1 || MODE == 3)
      {
        voice[V3].CC(midi.Control);
      }
    }
  }

  if (midi.AftertouchOn == true)
  {
    if (midi.Aftertouch == AFTERTOUCH)
    {
      if (MODE == 1)
      {
        voice[V2].CC(midi.Aftertouch);
      }
    }
  }

  if (midi.ClockOn == true) // Clock
  {
    digitalWrite(CLOCK_PIN, HIGH);
  }
  else if (midi.ClockOff == true)
  {
    digitalWrite(CLOCK_PIN, LOW);
  }
}


void NoteOn(byte midiNum, byte velocity) // Sets voltage to next available voice.
{
  for (int i = 0; i < voices; i++) {
    if (voice[i].noteState == false)
    {
      if (MODE == 5)
      {
        voice[V0].NoteOn(midiNum);
        voice[V1].NoteOn(midiNum);
        voice[V2].NoteOn(midiNum);
        voice[V3].NoteOn(midiNum);
      }
      else
      {
        voice[i].NoteOn(midiNum);
        voice[i].VelocityOn(velocity);
      }

      return;
    }
    else if (i == (voices - 1))
    {
      for (int u = 0; u < 8; u++)
      {
        if (UnusedNotes[u] == 128)
        {
          UnusedNotes[u] = voice[i].MidiNum;
          break;
        }
      }

      if (MODE == 5)
      {
        voice[V0].NoteOn(midiNum);
        voice[V1].NoteOn(midiNum);
        voice[V2].NoteOn(midiNum);
        voice[V3].NoteOn(midiNum);
      }
      else
      {
        voice[i].NoteOn(midiNum);
        voice[i].VelocityOn(velocity);
      }
    }
  }
}

void NoteOff(byte midiNum)
{
  for (int i = 0; i < voices; i++) {
    if (voice[i].noteState == true && voice[i].MidiNum == midiNum)
    {
      if (MODE == 5)
      {
        voice[V0].NoteOff();
        voice[V1].NoteOff();
        voice[V2].NoteOff();
        voice[V3].NoteOff();
      }
      else
      {
        voice[i].NoteOff();
      }

      for (int u = 7; u >= 0; u--) {
        {
          if (UnusedNotes[u] != 128)
          {
            if (MODE == 5)
            {
              voice[V0].NoteOn(UnusedNotes[u]);
              voice[V1].NoteOn(UnusedNotes[u]);
              voice[V2].NoteOn(UnusedNotes[u]);
              voice[V3].NoteOn(UnusedNotes[u]);
            }
            else
            {
              voice[i].NoteOn(UnusedNotes[u]);
            }

            UnusedNotes[u] = 128;
            break;
          }
        }
      }

      return;
    }
  }

  for (int i = 0; i < 8; i++)
  {
    if (UnusedNotes[i] == midiNum)
    {
      UnusedNotes[i] = 128;
      break;
    }
  }

  for (int i = 0; i < cvInputs; i++)
  {
    cvLevels[i] = floor(analogRead(analogInputs[i]) / 8); // convert 10 bit analog input to 7 bit MIDI range
    // if cv has changed, update cv input value and send new midi cc value
    if (cvLevels[i] != midiCCvalues[i])
    {
      midiCCvalues[i] = cvLevels[i];
      sendCC(midiChannel, midiCCnumbers[i], midiCCvalues[i]);
      delay(10); // need to experiment with this delay some more, 50 is probably too long
    }
  }
}
void sendCC(uint8_t channel, uint8_t control, uint8_t value) {
  Serial.write(0xB0 | (channel & 0xf));
  Serial.write(control & 0x7f);
  Serial.write(value & 0x7f);
}
