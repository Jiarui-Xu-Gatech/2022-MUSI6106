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

    Error_t init(float fFreqInHz, float fAmplitude, float fSamplerate)
    {
        // Set all the internal variables
        m_fFreqInHz = fFreqInHz;
        m_fAmplitude = fAmplitude;
        m_fSampleRate = fSamplerate;

        m_pfWaveTable = new CRingBuffer<float> (m_iWaveTableLength);
        m_pfLFOBuffer = new CRingBuffer<float>  ((int)(m_fSampleRate/m_fFreqInHz)); // TODO: Check what the length of
                                                                                        // the buffer should be

        // Create Wavetable and LFO
        createWaveTable();
        generateLFO();

        m_bIsInitialized = true;
    }

    CRingBuffer<float>* getLFO()
    {
        return m_pfLFOBuffer;
    }

private:
    CRingBuffer<float>* m_pfLFOBuffer = nullptr;
    CRingBuffer<float>* m_pfWaveTable = nullptr;
    bool m_bIsInitialized = false;
    int m_iWaveTableLength = 512;
    float m_fFreqInHz;
    float m_fAmplitude;
    float m_fSampleRate;

    Error_t createWaveTable()
    {
        for (int i=0; i< m_iWaveTableLength; i++)
        {
            m_pfWaveTable ->putPostInc(std::sin(2 * M_PI * i / m_iWaveTableLength));
        }
        return Error_t::kNoError;
    }

    Error_t generateLFO()
    {
        int iLengthLfo = (int)(m_fSampleRate/m_fFreqInHz);
        float index = 0;
        float indexInc = m_fFreqInHz * (float)m_iWaveTableLength / m_fSampleRate;

        for (int i=0; i < iLengthLfo; i++)
        {
            m_pfLFOBuffer ->putPostInc(m_pfWaveTable -> get(index) * m_fAmplitude);
            index += indexInc; // This will wrap around itself due to the ring buffer
        }

        return Error_t::kNoError;
    }


};


#endif /* Lfo_h */

