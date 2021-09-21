/// @file BGT60AiPMKR.h
/// @author Geike Hannes Mathias (IFAG PSS RFS 3DS RS PM) <HannesMathias.Geike@infineon.com>
/// @date 11-02-2020
/// @version V0.0

#ifndef BGT60AiPMKR_H
#define BGT60AiPMKR_H

/// Driver class for BGT60LTR11AiP Shield in autonoumus mode for 
/// the ARDUINO MKR WIFI 1010 or similiar (Based on Arduino library functions).
/// It provides comprehensive interface methods to control and read the BGT60LTR11AiP Shield
/// with the ARDUINO MKR WIFI 1010 or similiar in multiple configurations.
/// CONFIG 0: (default) Shield mounted on ARDUINO MKR WIFI 1010
/// CONFIG 1: Shield connected to ARDUINO MKR WIFI 1010 digital input pins
/// CONFIG 2: Shield connected to ARDUINO MKR WIFI 1010 digital input pins with an 
/// additional PMOS power switch between VCC and VCC(Shield) to turn shield on and off.
class BGT60AiPMKR
{
    /// Auxiliary enum class to provide information about TD and PD pin type.
    enum PinType
    {
        ANALOG,
        DIGITAL
    };

    /// Auxiliary structure to hold pin information about TD and PD.
    struct PinInfo
    {
        PinInfo(unsigned char pn, PinType pt, bool hc, bool cv, unsigned long ts) 
            : pinNumber{pn}, pinType{pt}, hasChanged{hc}, currValue{cv}, timeStamp{ts} {};
        const unsigned char pinNumber = 0;  // Pin number of input pin
        const BGT60AiPMKR::PinType pinType = ANALOG;      // Pin type of input pin
        bool hasChanged;        // Value changed after last read-out flag
        bool currValue;         // Current pin value flag
        unsigned long timeStamp;    // Timestamp of last value change ATTENTION!!! Does not regard overflow!
    };

    private:
        PinInfo* tdInfo;     // TD pin info structure
        PinInfo* pdInfo;     // PD pin info structure

        const int enPin;   // VCC enable pin number
        bool isEnabled;              // VCC is enabled flag
        unsigned char analogRes;     // ADC resolution in bit
        unsigned analogTh;           // ADC threshold value to determine pin logic value

        const unsigned DEFAULT_ADC_RES = 10u;    // Default ADC resolution
        const unsigned char DEFAULT_TD_PIN = 0x10;    // Equals Arduino Pin A1
        const unsigned char DEFAULT_PD_PIN = 0x11;    // Equals Arduino Pin A2

    public:
        /// BGT60AiPMKR default constructor
        /// Creates BGT60AiP Shield object for a mounted shield.
        /// (TD -> A1, PD -> A2, VCC(Shield) -> VCC(Arduino), GND(Shield) -> GND(Arduino))
        BGT60AiPMKR();

        /// BGT60AiPMKR digital pin input constructor
        /// Creates BGT60AiP shield object for a shield connected to the digital input pins of the 
        /// Arduino MKR without enable switch.
        /// (TD -> tdpin, PD -> pdpin, VCC(Shield) -> VCC(Arduino), GND(Shield) -> GND(Arduino))
        /// @param tdpin Digital input pin number of shields TD pin
        /// @param pdpin Digital input pin number of shields PD pin
        BGT60AiPMKR(unsigned char tdpin, unsigned char pdpin);

        /// BGT60AiPMKR digital pin input constructor
        /// Creates BGT60AiP shield object for a shield connected to the digital input pins of the 
        /// Arduino MKR with enable switch.
        /// (TD -> tdpin, PD -> pdpin, VCC(Shield) -> VCC(Arduino), GND(Shield) -> GND(Arduino),
        /// Switch -> enpin(Arduino))
        /// @param tdpin Digital input pin number of shields TD pin
        /// @param pdpin Digital input pin number of shields PD pin
        /// @param enpin Digital output pin number connected to the power switch of the shield
        BGT60AiPMKR(unsigned char tdpin, unsigned char pdpin, unsigned char enpin);
                


        bool getTd(void) const{ return tdInfo->currValue; }
        bool getPd(void) const{ return pdInfo->currValue; }
        bool getTdHasChanged(void) const{ return tdInfo->hasChanged; }
        bool getPdHasChanged(void) const{ return pdInfo->hasChanged; }
        unsigned long getTdTimeStamp(void) const { return tdInfo->timeStamp; }
        unsigned long getPdTimeStamp(void) const { return pdInfo->timeStamp; }

        unsigned char getADCResolution(void) const { return analogRes; }
        unsigned getADCTh(void) const { return analogTh; };
        
        bool getIsEnabled(void) const { return isEnabled; };

        void setTdHasChanged(bool hc){ tdInfo->hasChanged = hc; }
        void setPdHasChanged(bool hc){ pdInfo->hasChanged = hc; }

        void setIsEnabled(bool ie);

        /// Enables the shield by turning the power switch on if connected.
        inline void enable(void);

        /// Enables the shield by turning the power switch on if connected.
        inline void disable(void);

        /// Provides the current TD pin value and automatically mark the pin info as 
        /// as up-2-date by resetting the pin hasChanged flag.
        /// @returns Current TD pin value
        bool getTdValueWReset(void);

        /// Provides the current PD pin value and automatically mark the pin info as 
        /// as up-2-date by resetting the pin hasChanged flag.
        /// @returns Current PD pin value
        bool getPdValueWReset(void);

        /// Acquires the current pin information from the TD and PD input pins.
        /// In case a power switch is used to enable the shield the shield must be
        /// enabled first.
        /// @return : true in case shield was enabled and pin values has been acquired
        ///         : false in case the shield wasn't enabled
        bool read(void);


        /// Sets ADC resultion in case analog inputs are used to get TD and PD pin values.
        /// The ADC value threshold to logic value determination of the input pins is automatically
        /// set to the half of the ADC value range (2^res/2 - 1).
        /// ATTENTION! Changing the ADC resolution for the shield also changes the general 
        /// MKR board ADC resoltion!
        /// @param res ADC resultion in bit
        void setADCResolution(unsigned res);


    private:
        /// Gets initial pin values for TD and PD afer instance creation
        void initPinValues(void);

        /// Acquires the TD pin information of the shield instance with 
        /// analog TD input pin configuration.
        void readTdInfoAnalog(void);

        /// Acquires the TD pin information of the shield instance with 
        /// digital TD input pin configuration.
        void readPdInfoDigital(void);

        /// Acquires the PD pin information of the shield instance with 
        /// analog PD input pin configuration.
        void readPdInfoAnalog(void);

        /// Acquires the PD pin information of the shield instance with 
        /// digital PD input pin configuration.
        void readTdInfoDigital(void);

        /// Sets ADC threshold value to convert TD or PD analog pin values into
        /// a logical pin value. The ADC threshold value is dervied by the set
        /// ADC resolution value (2^(this->analogRes)/2 - 1).
        void setADCThreshold(void);
};

#endif /* BGT60AiPMKR_H */
