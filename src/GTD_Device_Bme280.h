
#include "GTD_Device_Bme280Config.h"
#include "GTD_I2C_Minimal.h"

class GTD_Device_Bme280
{
public:
    /*! @brief  sampling rates */
    enum sensor_sampling
    {
        SAMPLING_NONE = 0b000,
        SAMPLING_X1 = 0b001,
        SAMPLING_X2 = 0b010,
        SAMPLING_X4 = 0b011,
        SAMPLING_X8 = 0b100,
        SAMPLING_X16 = 0b101
    };

    /*! @brief  power modes */
    enum sensor_mode
    {
        MODE_SLEEP = 0b00,
        MODE_FORCED = 0b01,
        MODE_NORMAL = 0b11
    };

    /*! @brief  filter values */
    enum sensor_filter
    {
        FILTER_OFF = 0b000,
        FILTER_X2 = 0b001,
        FILTER_X4 = 0b010,
        FILTER_X8 = 0b011,
        FILTER_X16 = 0b100
    };

    /*! @brief  standby duration in ms */
    enum standby_duration
    {
        STANDBY_MS_0_5 = 0b000,
        STANDBY_MS_10 = 0b110,
        STANDBY_MS_20 = 0b111,
        STANDBY_MS_62_5 = 0b001,
        STANDBY_MS_125 = 0b010,
        STANDBY_MS_250 = 0b011,
        STANDBY_MS_500 = 0b100,
        STANDBY_MS_1000 = 0b101
    };

    // constructors
    GTD_Device_Bme280() {}

    ~GTD_Device_Bme280(void)
    {
        if (i2c)
        {
            delete i2c;
        }
    }

    bool begin(uint8_t addr = BME280_ADDRESS, TwoWire *theWire = &Wire)
    {
        if (i2c)
            delete i2c;
        i2c = new GTD_I2C_Minimal(addr, theWire);
        if (!i2c->begin())
        {
            return false;
        }
    }

    bool init()
    {
        _sensorID = read8(BME280_REGISTER_CHIPID);
        // if (_sensorID != 0x60){
        //   return false;
        // }

        // reset the device using soft-reset
        // this makes sure the IIR is off, etc.
        write8(BME280_REGISTER_SOFTRESET, 0xB6);
        delay(10);

        // if chip is still reading calibration, delay
        while (isReadingCalibration())
        {
            delay(10);
        }

        readCoefficients(); // read trimming parameters, see DS 4.2.2
        setSampling();      // use defaults
        delay(100);

        return true;
    };

    void setSampling(sensor_mode mode = MODE_NORMAL,
                     sensor_sampling tempSampling = SAMPLING_X16,
                     sensor_sampling pressSampling = SAMPLING_X16,
                     sensor_sampling humSampling = SAMPLING_X16,
                     sensor_filter filter = FILTER_OFF,
                     standby_duration duration = STANDBY_MS_0_5)
    {
        _measReg.mode = mode;
        _measReg.osrs_t = tempSampling;
        _measReg.osrs_p = pressSampling;

        _humReg.osrs_h = humSampling;
        _configReg.filter = filter;
        _configReg.t_sb = duration;
        _configReg.spi3w_en = 0;

        // making sure sensor is in sleep mode before setting configuration
        // as it otherwise may be ignored
        write8(BME280_REGISTER_CONTROL, MODE_SLEEP);

        // you must make sure to also set REGISTER_CONTROL after setting the
        // CONTROLHUMID register, otherwise the values won't be applied (see
        // DS 5.4.3)
        write8(BME280_REGISTER_CONTROLHUMID, _humReg.get());
        write8(BME280_REGISTER_CONFIG, _configReg.get());
        write8(BME280_REGISTER_CONTROL, _measReg.get());
    }

    bool takeForcedMeasurement(void);

    float readTemperature(void)
    {
        int32_t var1, var2;

        int32_t adc_T = read24(BME280_REGISTER_TEMPDATA);
        if (adc_T == 0x800000) // value in case temp measurement was disabled
            return NAN;
        adc_T >>= 4;

        var1 = (int32_t)((adc_T / 8) - ((int32_t)_bme280_calib.dig_T1 * 2));
        var1 = (var1 * ((int32_t)_bme280_calib.dig_T2)) / 2048;
        var2 = (int32_t)((adc_T / 16) - ((int32_t)_bme280_calib.dig_T1));
        var2 = (((var2 * var2) / 4096) * ((int32_t)_bme280_calib.dig_T3)) / 16384;

        t_fine = var1 + var2 + t_fine_adjust;

        int32_t T = (t_fine * 5 + 128) / 256;

        return (float)T / 100;
    }

    float readPressure(void)
    {
        int64_t var1, var2, var3, var4;

        readTemperature(); // must be done first to get t_fine

        int32_t adc_P = read24(BME280_REGISTER_PRESSUREDATA);
        if (adc_P == 0x800000) // value in case pressure measurement was disabled
            return NAN;
        adc_P >>= 4;

        var1 = ((int64_t)t_fine) - 128000;
        var2 = var1 * var1 * (int64_t)_bme280_calib.dig_P6;
        var2 = var2 + ((var1 * (int64_t)_bme280_calib.dig_P5) * 131072);
        var2 = var2 + (((int64_t)_bme280_calib.dig_P4) * 34359738368);
        var1 = ((var1 * var1 * (int64_t)_bme280_calib.dig_P3) / 256) +
               ((var1 * ((int64_t)_bme280_calib.dig_P2) * 4096));
        var3 = ((int64_t)1) * 140737488355328;
        var1 = (var3 + var1) * ((int64_t)_bme280_calib.dig_P1) / 8589934592;

        if (var1 == 0)
        {
            return 0; // avoid exception caused by division by zero
        }

        var4 = 1048576 - adc_P;
        var4 = (((var4 * 2147483648) - var2) * 3125) / var1;
        var1 = (((int64_t)_bme280_calib.dig_P9) * (var4 / 8192) * (var4 / 8192)) /
               33554432;
        var2 = (((int64_t)_bme280_calib.dig_P8) * var4) / 524288;
        var4 = ((var4 + var1 + var2) / 256) + (((int64_t)_bme280_calib.dig_P7) * 16);

        float P = var4 / 256.0;

        return P;
    }

    float readHumidity(void)
    {
        int32_t var1, var2, var3, var4, var5;

        readTemperature(); // must be done first to get t_fine

        int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);
        if (adc_H == 0x8000) // value in case humidity measurement was disabled
            return NAN;

        var1 = t_fine - ((int32_t)76800);
        var2 = (int32_t)(adc_H * 16384);
        var3 = (int32_t)(((int32_t)_bme280_calib.dig_H4) * 1048576);
        var4 = ((int32_t)_bme280_calib.dig_H5) * var1;
        var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
        var2 = (var1 * ((int32_t)_bme280_calib.dig_H6)) / 1024;
        var3 = (var1 * ((int32_t)_bme280_calib.dig_H3)) / 2048;
        var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
        var2 = ((var4 * ((int32_t)_bme280_calib.dig_H2)) + 8192) / 16384;
        var3 = var5 * var2;
        var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
        var5 = var3 - ((var4 * ((int32_t)_bme280_calib.dig_H1)) / 16);
        var5 = (var5 < 0 ? 0 : var5);
        var5 = (var5 > 419430400 ? 419430400 : var5);
        uint32_t H = (uint32_t)(var5 / 4096);

        return (float)H / 1024.0;
    }

    float readAltitude(float seaLevel)
    {
        float atmospheric = readPressure() / 100.0F;
        return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
    }

    float seaLevelForAltitude(float altitude, float pressure);

    uint32_t sensorID(void) { return _sensorID; }
    float getTemperatureCompensation(void)
    {
        return float((t_fine_adjust * 5) >> 8) / 100.0;
    }
    void setTemperatureCompensation(float adjustment)
    {
        // convert the value in C into and adjustment to t_fine
        t_fine_adjust = ((int32_t(adjustment * 100) << 8)) / 5;
    };

protected:
    GTD_I2C_Minimal *i2c = nullptr; ///< Pointer to I2C bus interface

    void readCoefficients(void)
    {
        _bme280_calib.dig_T1 = read16_LE(BME280_REGISTER_DIG_T1);
        _bme280_calib.dig_T2 = readS16_LE(BME280_REGISTER_DIG_T2);
        _bme280_calib.dig_T3 = readS16_LE(BME280_REGISTER_DIG_T3);

        _bme280_calib.dig_P1 = read16_LE(BME280_REGISTER_DIG_P1);
        _bme280_calib.dig_P2 = readS16_LE(BME280_REGISTER_DIG_P2);
        _bme280_calib.dig_P3 = readS16_LE(BME280_REGISTER_DIG_P3);
        _bme280_calib.dig_P4 = readS16_LE(BME280_REGISTER_DIG_P4);
        _bme280_calib.dig_P5 = readS16_LE(BME280_REGISTER_DIG_P5);
        _bme280_calib.dig_P6 = readS16_LE(BME280_REGISTER_DIG_P6);
        _bme280_calib.dig_P7 = readS16_LE(BME280_REGISTER_DIG_P7);
        _bme280_calib.dig_P8 = readS16_LE(BME280_REGISTER_DIG_P8);
        _bme280_calib.dig_P9 = readS16_LE(BME280_REGISTER_DIG_P9);

        _bme280_calib.dig_H1 = read8(BME280_REGISTER_DIG_H1);
        _bme280_calib.dig_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
        _bme280_calib.dig_H3 = read8(BME280_REGISTER_DIG_H3);
        _bme280_calib.dig_H4 = ((int8_t)read8(BME280_REGISTER_DIG_H4) << 4) |
                               (read8(BME280_REGISTER_DIG_H4 + 1) & 0xF);
        _bme280_calib.dig_H5 = ((int8_t)read8(BME280_REGISTER_DIG_H5 + 1) << 4) |
                               (read8(BME280_REGISTER_DIG_H5) >> 4);
        _bme280_calib.dig_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);
    }
    bool isReadingCalibration(void)
    {
        uint8_t const rStatus = read8(BME280_REGISTER_STATUS);
        return (rStatus & (1 << 0)) != 0;
    }

    void write8(byte reg, byte value)
    {
        byte buffer[2];
        buffer[1] = value;
        buffer[0] = reg;
        i2c->write(buffer, 2);
    }

    uint8_t read8(byte reg)
    {
        uint8_t buffer[1];
        buffer[0] = uint8_t(reg);
        i2c->write_then_read(buffer, 1, buffer, 1);
        return buffer[0];
    }

    uint16_t read16(byte reg)
    {
        uint8_t buffer[2];

        buffer[0] = uint8_t(reg);
        i2c->write_then_read(buffer, 1, buffer, 2);
        return uint16_t(buffer[0]) << 8 | uint16_t(buffer[1]);
    }

    uint32_t read24(byte reg)
    {
        uint8_t buffer[3];

        buffer[0] = uint8_t(reg);
        i2c->write_then_read(buffer, 1, buffer, 3);

        return uint32_t(buffer[0]) << 16 | uint32_t(buffer[1]) << 8 |
               uint32_t(buffer[2]);
    }
    int16_t readS16(byte reg)
    {
        return (int16_t)read16(reg);
    }
    uint16_t read16_LE(byte reg) // little endian
    {
        uint16_t temp = read16(reg);
        return (temp >> 8) | (temp << 8);
    }
    int16_t readS16_LE(byte reg) // little endian
    {
        return (int16_t)read16_LE(reg);
    }

    uint8_t _i2caddr;  //!< I2C addr for the TwoWire interface
    int32_t _sensorID; //!< ID of the BME Sensor
    int32_t t_fine;    //!< temperature with high resolution, stored as an attribute
                       //!< as this is used for temperature compensation reading
                       //!< humidity and pressure

    int32_t t_fine_adjust = 0; //!< add to compensate temp readings and in turn
                               //!< to pressure and humidity readings

    bme280_calib_data _bme280_calib; //!< here calibration data is stored

    /*! @brief  config register */
    struct config
    {
        // inactive duration (standby time) in normal mode
        // 000 = 0.5 ms
        // 001 = 62.5 ms
        // 010 = 125 ms
        // 011 = 250 ms
        // 100 = 500 ms
        // 101 = 1000 ms
        // 110 = 10 ms
        // 111 = 20 ms
        unsigned int t_sb : 3; ///< inactive duration (standby time) in normal mode

        // filter settings
        // 000 = filter off
        // 001 = 2x filter
        // 010 = 4x filter
        // 011 = 8x filter
        // 100 and above = 16x filter
        unsigned int filter : 3; ///< filter settings

        // unused - don't set
        unsigned int none : 1;     ///< unused - don't set
        unsigned int spi3w_en : 1; ///< unused - don't set

        /// @return combined config register
        unsigned int get() { return (t_sb << 5) | (filter << 2) | spi3w_en; }
    };
    config _configReg; //!< config register object

    /*! @brief  ctrl_meas register */
    struct ctrl_meas
    {
        // temperature oversampling
        // 000 = skipped
        // 001 = x1
        // 010 = x2
        // 011 = x4
        // 100 = x8
        // 101 and above = x16
        unsigned int osrs_t : 3; ///< temperature oversampling

        // pressure oversampling
        // 000 = skipped
        // 001 = x1
        // 010 = x2
        // 011 = x4
        // 100 = x8
        // 101 and above = x16
        unsigned int osrs_p : 3; ///< pressure oversampling

        // device mode
        // 00       = sleep
        // 01 or 10 = forced
        // 11       = normal
        unsigned int mode : 2; ///< device mode

        /// @return combined ctrl register
        unsigned int get() { return (osrs_t << 5) | (osrs_p << 2) | mode; }
    };
    ctrl_meas _measReg; //!< measurement register object

    /*! @brief  ctrl_hum register */
    struct ctrl_hum
    {
        /// unused - don't set
        unsigned int none : 5;

        // pressure oversampling
        // 000 = skipped
        // 001 = x1
        // 010 = x2
        // 011 = x4
        // 100 = x8
        // 101 and above = x16
        unsigned int osrs_h : 3; ///< pressure oversampling

        /// @return combined ctrl hum register
        unsigned int get() { return (osrs_h); }
    };
    ctrl_hum _humReg; //!< hum register object
};