#include "Vibrato.h"



CVibratoIf::CVibratoIf():
m_bIsInitialized(false),
m_fSampleRate(0),
m_fMaxDelayLengthInS(0),
m_iNumChannels(0)

{
    this -> reset();
}

CVibratoIf::~CVibratoIf()
{
    this -> reset();
}

Error_t CVibratoIf::create (CVibratoIf*& pCVibratoIF)
{
    pCVibratoIF = new CVibratoIf();
    return Error_t::kNoError;
}
    

Error_t CVibratoIf::destroy (CVibratoIf*& pCVibratoIF)
{
    pCVibratoIF -> reset();
    delete pCVibratoIF;
    pCVibratoIF = nullptr;
    return Error_t::kNoError;
}
    

Error_t CVibratoIf::init(float fMaxDelayLengthInS,float fFreqModInHz, int iNumChannels, float fSampleRateInHz, float fAmplitude)
{
    // Initialize Vibrato variables
    m_fMaxDelayLengthInS = fMaxDelayLengthInS;
    m_fSampleRate = fSampleRateInHz;
    m_iNumChannels = iNumChannels;


    // Initialize LFO
    CLFO::create(pCLFO);
    pCLFO -> init(fFreqModInHz, fAmplitude, fSampleRateInHz);
    pLFOBuffer = pCLFO -> getLFO();

    // initialize RingBuffer for delayLine
    pDelayLine = new CRingBuffer<float>* [iNumChannels];
    int iMaxDelayInSamples = int(fMaxDelayLengthInS * fSampleRateInHz);
    int iDelayLineLength = 2 + (int)iMaxDelayInSamples + (int)fAmplitude * 2;

    for (int i=0; i< iNumChannels; i++)
    {
        pDelayLine[i] = new CRingBuffer<float> (iDelayLineLength);
        pDelayLine[i] -> reset();
    }
    return Error_t::kNoError;
}

    
Error_t CVibratoIf::reset ()
{
    m_fSampleRate = 0;
    m_bIsInitialized= false;
    return Error_t::kNoError;
}


Error_t CVibratoIf::process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    float fMaxDelayLengthInSamples = m_fMaxDelayLengthInS * m_fSampleRate;
    for (int i=0; i < m_iNumChannels; i++)
    {
        for (int j=0; j < iNumberOfFrames; j++)
        {
            pDelayLine[i] -> putPostInc(ppfInputBuffer[i][j]);
            float delaySamps = pLFOBuffer -> getPostInc() * fMaxDelayLengthInSamples;
            ppfOutputBuffer[i][j] = pDelayLine[i] -> get(delaySamps);
        }
    }
}



