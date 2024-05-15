#pragma once
#include "GTD_Device_Pca9685_Config.h"

class GTD_Device_Pca9685
{
public:
    GTD_Device_Pca9685() : _i2caddr(PCA9685_I2C_ADDRESS) {}

    GTD_Device_Pca9685(const uint8_t addr) : _i2caddr(addr) {}

    bool begin(TwoWire &tw, uint8_t prescale = 0)
    {
        if (i2c_dev)
            delete i2c_dev;
        i2c_dev = new GTD_I2C_Minimal(_i2caddr, &tw);
        if (!i2c_dev->begin())
            return false;
        reset();

        // set the default internal frequency
        setOscillatorFrequency(FREQUENCY_OSCILLATOR);

        if (prescale)
        {
            setExtClk(prescale);
        }
        else
        {
            // set a default frequency
            setPWMFreq(1000);
        }

        return true;
    }

    void reset()
    {
        write8(PCA9685_MODE1, MODE1_RESTART);
        delay(10);
    }

    void sleep()
    {
        uint8_t awake = read8(PCA9685_MODE1);
        uint8_t sleep = awake | MODE1_SLEEP; // set sleep bit high
        write8(PCA9685_MODE1, sleep);
        delay(5); // wait until cycle ends for sleep to be active
    }

    void wakeup()
    {
        uint8_t sleep = read8(PCA9685_MODE1);
        uint8_t wakeup = sleep & ~MODE1_SLEEP; // set sleep bit low
        write8(PCA9685_MODE1, wakeup);
    }

    void setExtClk(uint8_t prescale)
    {
        uint8_t oldmode = read8(PCA9685_MODE1);
        uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
        write8(PCA9685_MODE1, newmode);                             // go to sleep, turn off internal oscillator

        // This sets both the SLEEP and EXTCLK bits of the MODE1 register to switch to
        // use the external clock.
        write8(PCA9685_MODE1, (newmode |= MODE1_EXTCLK));

        write8(PCA9685_PRESCALE, prescale); // set the prescaler

        delay(5);
        // clear the SLEEP bit to start
        write8(PCA9685_MODE1, (newmode & ~MODE1_SLEEP) | MODE1_RESTART | MODE1_AI);
    }

    void setPWMFreq(float freq)
    {
        // Range output modulation frequency is dependant on oscillator
        if (freq < 1)
            freq = 1;
        if (freq > 3500)
            freq = 3500; // Datasheet limit is 3052=50MHz/(4*4096)

        float prescaleval = ((_oscillator_freq / (freq * 4096.0)) + 0.5) - 1;
        if (prescaleval < PCA9685_PRESCALE_MIN)
            prescaleval = PCA9685_PRESCALE_MIN;
        if (prescaleval > PCA9685_PRESCALE_MAX)
            prescaleval = PCA9685_PRESCALE_MAX;
        uint8_t prescale = (uint8_t)prescaleval;

        uint8_t oldmode = read8(PCA9685_MODE1);
        uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
        write8(PCA9685_MODE1, newmode);                             // go to sleep
        write8(PCA9685_PRESCALE, prescale);                         // set the prescaler
        write8(PCA9685_MODE1, oldmode);
        delay(5);
        // This sets the MODE1 register to turn on auto increment.
        write8(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);
    }

    void setOutputMode(bool totempole)
    {
        uint8_t oldmode = read8(PCA9685_MODE2);
        uint8_t newmode;
        if (totempole)
        {
            newmode = oldmode | MODE2_OUTDRV;
        }
        else
        {
            newmode = oldmode & ~MODE2_OUTDRV;
        }
        write8(PCA9685_MODE2, newmode);
    }
    
    uint16_t getPWM(uint8_t num, bool off = false);

    uint8_t setPWM(uint8_t num, uint16_t on, uint16_t off)
    {
        uint8_t buffer[5];
        buffer[0] = PCA9685_LED0_ON_L + 4 * num;
        buffer[1] = on;
        buffer[2] = on >> 8;
        buffer[3] = off;
        buffer[4] = off >> 8;

        if (i2c_dev->write(buffer, 5))
        {
            return 0;
        }
        return 1;
    }

    void setPin(uint8_t num, uint16_t val, bool invert = false)
    {
        // Clamp value between 0 and 4095 inclusive.
        val = min(val, (uint16_t)4095);
        if (invert)
        {
            if (val == 0)
            {
                // Special value for signal fully on.
                setPWM(num, 4096, 0);
            }
            else if (val == 4095)
            {
                // Special value for signal fully off.
                setPWM(num, 0, 4096);
            }
            else
            {
                setPWM(num, 0, 4095 - val);
            }
        }
        else
        {
            if (val == 4095)
            {
                // Special value for signal fully on.
                setPWM(num, 4096, 0);
            }
            else if (val == 0)
            {
                // Special value for signal fully off.
                setPWM(num, 0, 4096);
            }
            else
            {
                setPWM(num, 0, val);
            }
        }
    }

    uint8_t readPrescale(void)
    {
        return read8(PCA9685_PRESCALE);
    }

    void writeMicroseconds(uint8_t num, uint16_t Microseconds)
    {
        double pulse = Microseconds;
        double pulselength;
        pulselength = 1000000; // 1,000,000 us per second

        // Read prescale
        uint16_t prescale = readPrescale();

        // Calculate the pulse for PWM based on Equation 1 from the DS 7.3.5
        prescale += 1;
        pulselength *= prescale;
        pulselength /= _oscillator_freq;
        pulse /= pulselength;

        setPWM(num, 0, pulse);
    }

    void setOscillatorFrequency(uint32_t freq)
    {
        _oscillator_freq = freq;
    }
    uint32_t getOscillatorFrequency(void)
    {
        return _oscillator_freq;
    }

private:
    uint8_t _i2caddr;
    GTD_I2C_Minimal *i2c_dev = nullptr; ///< Pointer to I2C bus interface

    uint32_t _oscillator_freq;

    uint8_t read8(uint8_t addr)
    {
        uint8_t buffer[1] = {addr};
        i2c_dev->write_then_read(buffer, 1, buffer, 1);
        return buffer[0];
    }
    void write8(uint8_t addr, uint8_t d)
    {
        uint8_t buffer[2] = {addr, d};
        i2c_dev->write(buffer, 2);
    }
};