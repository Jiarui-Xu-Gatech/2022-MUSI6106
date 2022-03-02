#include "RingBuffer.h"
#include "Vibrato.h"


#ifndef __Lfo__H
#define __Lfo__H

class LfoWavetable {
public:
	LfoWavetable(int delaySamps, int m_fSampleRate, float fFrequency, float fMaxDelayLengthInSamples) :
		m_fSampleRate(m_fSampleRate),
		m_fReadIdx(0.f)
	{
        pLfoRingBuff = new CRingBuffer<float>(delaySamps);
        for (int i = 0; i < delaySamps; i++)
        {
            pLfoRingBuff->putPostInc(sin(2 * M_PI * i / delaySamps));
        }
        m_fAmplitude = fMaxDelayLengthInSamples;
        m_fReadInc = static_cast<float>(m_fSampleRate) / static_cast<float>(pLfoRingBuff->getLength()) / fFrequency;
	}

	virtual ~LfoWavetable() {
		delete[] pLfoRingBuff;
	}

private:
	CRingBuffer<float>* pLfoRingBuff = 0;
	int m_fSampleRate;
	float m_fReadIdx;
	float m_fAmplitude;
	float m_fReadInc;
};





#endif//__Lfo__H
