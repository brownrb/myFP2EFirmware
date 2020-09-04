// PIN STATE SEQUENCES FOR STEPS
// Two pin step motor sequences
// NOTE: Only two variations exist; dual phasing is the same as half-stepping.

/*
// Four pin step motor sequences
// Structured by Stepping mode --> Phasing mode --> Sequence type.
PROGMEM static const byte _STEP_SEQUENCES_FOUR_PIN[2][2][2][8] =
{
    {
        {
            { B00001000, B00000010, B00000100, B00000001, B00001000, B00000010, B00000100, B00000001 },
            { B00001000, B00000100, B00000010, B00000001, B00001000, B00000100, B00000010, B00000001 }
        },
        {
            { B00001100, B00000110, B00000110, B00000011, B00001100, B00000110, B00000110, B00000011 },
            { B00001010, B00000110, B00000101, B00001001, B00001010, B00000110, B00000101, B00001001 }
        }
    },
    {
        {
            { B00001000, B00001100, B00000100, B00000110, B00000010, B00000011, B00000001, B00001001 },
            { B00001000, B00001010, B00000010, B00000110, B00000100, B00000101, B00000001, B00001001 }
        },
        {
            { B00001000, B00001100, B00000100, B00000110, B00000010, B00000011, B00000001, B00001001 },
            { B00001000, B00001010, B00000010, B00000110, B00000100, B00000101, B00000001, B00001001 }
        }
        
    };
}

_Steps = new byte[4];

for (int i = 0; i < 4; i++)
    _Steps[i] = pgm_read_byte_near(&_STEP_SEQUENCES_TWO_PIN[BOOL_TO_INDEX((bool)_SteppingMode)][i]);



DoStep(byte stepIdx)
    if (_PinCount == 4)
    {
        digitalWrite(_Pins[0], _Steps[stepIdx] & B1000 ? HIGH : LOW);
        digitalWrite(_Pins[1], _Steps[stepIdx] & B0100 ? HIGH : LOW);
        digitalWrite(_Pins[2], _Steps[stepIdx] & B0010 ? HIGH : LOW);
        digitalWrite(_Pins[3], _Steps[stepIdx] & B0001 ? HIGH : LOW);
    }

void HalfStepper::Step(long numSteps)
{
    if (numSteps == 0)
        return;

    if (numSteps > 0)
    {
        _Direction = Direction::FORWARD;
    }
    else
    {
        _Direction = Direction::REVERSE;
        numSteps = abs(numSteps);
    }

    while (numSteps-- > 0)
    {
        while (millis() - _LastStepMS < _DelayMS) delay(1);

        _LastStepMS = millis();

        if (_Direction == Direction::FORWARD)
        {
            if (_Position++ == _TotalSteps)
                _Position = 0;
        }
        else
        {
            if (_Position-- == 0)
                _Position = _TotalSteps;
        }

        this->DoStep(_Position % (_PinCount * 2));
    }
}
*/