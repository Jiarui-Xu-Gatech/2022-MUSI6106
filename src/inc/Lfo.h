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

class CLFO
{
public:

    static Error_t create(CLFO*& pCLFO)
    {
        pCLFO = new CLFO();
        return Error_t::kNoError;
    }

    static Error_t destroy(CLFO*& pCLFO)
    {
        delete pCLFO;
        pCLFO = nullptr;
        return Error_t::kNoError;
    }

    static Error_t reset()
    {
        // TODO: write the reset function
    }

    Error_t init(float fFreqInSamples, float fAmplitude, float fSamplerate)
    {
        // Set all the internal variables
        m_fFreqInSamples = fFreqInSamples;
        m_fAmplitude = fAmplitude;
        m_fSampleRate = fSamplerate;

        m_pfWaveTable = new CRingBuffer<float> (m_iWaveTableLength);

        // Create Wavetable
        createWaveTable();
        m_bIsInitialized = true;
    }

    float process()
    {
        float fCurrValue = m_pfWaveTable -> get(m_fCurrIdx);
        float fIdxInc = m_fFreqInSamples * m_iWaveTableLength / m_fSampleRate ;

        while ((fCurrValue += fIdxInc) > m_iWaveTableLength)
            m_fCurrIdx -= m_iWaveTableLength;
        return fCurrValue * m_fAmplitude;
    };


private:
    CRingBuffer<float>* m_pfWaveTable;
    bool m_bIsInitialized;
    int m_iWaveTableLength = 2048;
    float m_fFreqInSamples;
    float m_fAmplitude;
    float m_fSampleRate;
    float m_fCurrIdx;

    Error_t createWaveTable()
    {
        for (int i=0; i< m_iWaveTableLength; i++)
        {
            m_pfWaveTable ->putPostInc(std::sin(2 * M_PI * i / m_iWaveTableLength));
        }
        return Error_t::kNoError;
    }
};



#endif /* Lfo_h */

