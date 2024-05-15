#pragma once
#include "GTD_config.h"


class GTD_I2C_Minimal
{
public:
    GTD_I2C_Minimal(uint8_t addr, TwoWire *theWire)
    {
        _addr = addr;
        _wire = theWire;
        _begun = false;
#ifdef ESP32
        _maxBufferSize = I2C_BUFFER_LENGTH;
#else
        _maxBufferSize = 255;
#endif
    }

    uint8_t address(void)
    {
        return _addr;
    }

    bool begin(bool addr_detect = true)
    {
        _wire->begin();
        _begun = true;

        if (addr_detect)
        {
            return detected();
        }
        return true;
    }

    void end(void)
    {
        _begun = false;
        Wire.end();
    };

    bool detected(void)
    {
        // Init I2C if not done yet
        if (!_begun && !begin())
        {
            return false;
        }

        // A basic scanner, see if it ACK's
        _wire->beginTransmission(_addr);
        if (_wire->endTransmission() == 0)
        {
            return true;
        }
        return false;
    }

    bool read(uint8_t *buffer, size_t len, bool stop = true)
    {
        size_t pos = 0;
        while (pos < len)
        {
            size_t read_len =
                ((len - pos) > _maxBufferSize) ? _maxBufferSize : (len - pos);
            bool read_stop = (pos < (len - read_len)) ? false : stop;
            if (!_read(buffer + pos, read_len, read_stop))
                return false;
            pos += read_len;
        }
        return true;
    }

    bool write(const uint8_t *buffer, size_t len, bool stop = true,
               const uint8_t *prefix_buffer = nullptr, size_t prefix_len = 0)
    {
        if ((len + prefix_len) > _maxBufferSize)
        {
            return false;
        }

        _wire->beginTransmission(_addr);

        // Write the prefix data (usually an address)
        if ((prefix_len != 0) && (prefix_buffer != nullptr))
        {
            if (_wire->write(prefix_buffer, prefix_len) != prefix_len)
            {
                return false;
            }
        }

        // Write the data itself
        if (_wire->write(buffer, len) != len)
        {
            return false;
        }

        if (_wire->endTransmission(stop) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool write_then_read(const uint8_t *write_buffer, size_t write_len,
                         uint8_t *read_buffer, size_t read_len,
                         bool stop = false)
    {
        if (!write(write_buffer, write_len, stop))
        {
            return false;
        }
        return read(read_buffer, read_len);
    }

    bool setSpeed(uint32_t desiredclk)
    {
#if (ARDUINO >= 157) && !defined(ARDUINO_STM32_FEATHER) && \
    !defined(TinyWireM_h)
        _wire->setClock(desiredclk);
        return true;
#else
        (void)desiredclk;
        return false;
#endif
    }

    size_t maxBufferSize() { return _maxBufferSize; }

private:
    uint8_t _addr;
    TwoWire *_wire;
    bool _begun;
    size_t _maxBufferSize;

    bool _read(uint8_t *buffer, size_t len, bool stop)
    {
        size_t recv = _wire->requestFrom((uint8_t)_addr, (uint8_t)len, (uint8_t)stop);

        if (recv != len)
        {
            // Not enough data available to fulfill our obligation!
            return false;
        }

        for (uint16_t i = 0; i < len; i++)
        {
            buffer[i] = _wire->read();
        }

        return true;
    }
};
