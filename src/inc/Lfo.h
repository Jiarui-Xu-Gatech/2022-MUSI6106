//
//  Lfo.hpp
//  AudioFileIO
//
//  Created by Admin on 3/2/22.
//

#ifndef Lfo_h
#define Lfo_h

#include <stdio.h>
#include "RingBuffer.h"
#include "ErrorDef.h"
#include <math.h>

class CLfo
{
public:

    enum Params_t
    {
        kParamFreq,
        kParamAmplitude,
        kParamSampleRate,

        kNumParams
    };

    Error_t setParams(Params_t eParamType, float eParamValue)
    {
        switch (eParamType)
        {
            case (Params_t::kParamFreq):
                m_fFreqInHz = eParamValue;
            case (Params_t::kParamAmplitude):
                m_fAmplitude = eParamValue;
            case (Params_t::kParamSampleRate):
                m_fSampleRate = eParamValue;
            case (Params_t::kNumParams):
                return Error_t::kInvalidString;
        }
        return Error_t::kNoError;
    }

    float getParams(Params_t eParamType) const
    {
        switch (eParamType) {
            case (Params_t::kParamFreq):
                return m_fFreqInHz;
            case (Params_t::kParamAmplitude):
                return m_fAmplitude;
            case (Params_t::kParamSampleRate):
                return m_fSampleRate;
            case (Params_t::kNumParams):
                return Params_t::kNumParams;
        }
    }

    Error_t init(float fFreqInHz, float fAmplitude, float fSamplerate)
    {
        // Set all the internal variables
        m_fFreqInHz = fFreqInHz;
        m_fAmplitude = fAmplitude;
        m_fSampleRate = fSamplerate;

        // Create Wavetable and LFO
        createWaveTable();
        generateLFO();
        m_bIsInitialized = true;
    }


    Error_t createWaveTable()
    {
        m_pfWaveTable = new CRingBuffer<float> (m_iWaveTableLength);

        for (int i=0; i< m_iWaveTableLength; i++)
        {
            m_pfWaveTable ->putPostInc(std::sin(2 * M_PI * i / m_iWaveTableLength));
        }

        return Error_t::kNoError;
    }

    Error_t generateLFO()
    {
        int iLengthLfo = (int)m_fSampleRate;
        m_pfLFOBuffer = new CRingBuffer<float> (iLengthLfo);
        float index = 0;
        float indexInc = m_fFreqInHz * (float)m_iWaveTableLength / m_fSampleRate;

        for (int i=0; i < iLengthLfo; i++)
        {
            m_pfLFOBuffer ->putPostInc(m_pfWaveTable -> get(index) * m_fAmplitude);
            index += indexInc; // This will wrap around itself due to the ring buffer
        }

        return Error_t::kNoError;
    }

protected:
    CRingBuffer<float>* m_pfLFOBuffer = nullptr;

private:
    CRingBuffer<float>* m_pfWaveTable = nullptr;
    bool m_bIsInitialized = false;
    int m_iWaveTableLength = 512;
    float m_fFreqInHz;
    float m_fAmplitude;
    float m_fSampleRate;
};


#endif /* Lfo_h */

