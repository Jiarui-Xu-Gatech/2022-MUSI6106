#include "Vibrato.h"



CVibratoIf::CVibratoIf():
m_bIsInitialized(false),
m_fSampleRate(0),
m_fDelayInSamples(0),
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
    
Error_t CVibratoIf::setParam(VibratoParam_t eParam, float fParamValue)
{
    if (!m_bIsInitialized)
    {
        return Error_t::kNotInitializedError;
    }

    switch (eParam)
    {
        case kParamDelay:
            m_fDelayInSamples = fParamValue * m_fSampleRate;
            pCLFO ->setModAmplitude(fParamValue * m_fSampleRate);
            break;

        case kParamFreqMod:
            m_fFreqModInHz = fParamValue;
            pCLFO ->setModFreq(fParamValue);
            break;
    }
    return Error_t::kNoError;
}

float CVibratoIf::getParam(VibratoParam_t eParam) const
{
    if (!m_bIsInitialized)
    {
        return -1;
    }

    switch (eParam)
    {
        case kParamDelay:
            return m_fDelayInSamples / m_fSampleRate;

        case kParamFreqMod:
            return m_fFreqModInHz;
    }
}

Error_t CVibratoIf::init(int iNumChannels, float fSampleRateInHz, float fDelayInSec, float fFreqModInHz)
{

    // Initialize Vibrato variables
    m_fSampleRate = fSampleRateInHz;
    m_iNumChannels = iNumChannels;
    m_fDelayInSamples = std::round(fDelayInSec * fSampleRateInHz);

    // Initialize LFO
    CLFO::create(pCLFO);
    pCLFO -> init(fFreqModInHz, fDelayInSec, m_fSampleRate);

    // Initialize RingBuffer for delayLine
    pDelayLine = new CRingBuffer<float>* [iNumChannels];
    int iDelayLineLength = static_cast<int>( 2 * m_fDelayInSamples + 2);

    for (int i=0; i< iNumChannels; i++)
    {
        pDelayLine[i] = new CRingBuffer<float> (iDelayLineLength);
        pDelayLine[i] -> reset();
    }
    m_bIsInitialized = true;
    return Error_t::kNoError;
}

    
Error_t CVibratoIf::reset ()
{
    if (!m_bIsInitialized)
    {
        return Error_t::kNotInitializedError;
    }
    delete pCLFO;
    pCLFO = 0;
    delete pDelayLine;
    pDelayLine = 0;
    m_bIsInitialized = false;
    return Error_t::kNoError;
}


Error_t CVibratoIf::process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    if (!m_bIsInitialized)
    {
        return Error_t::kNotInitializedError;
    }

    for (int i=0; i < m_iNumChannels; i++)
    {
        for (int j=0; j < iNumberOfFrames; j++)
        {
            pDelayLine[i] -> putPostInc(ppfInputBuffer[i][j]);
            ppfOutputBuffer[i][j] = pDelayLine[i] -> get(pCLFO -> process());
            pDelayLine[i] -> getPostInc();
        }
    }
    return Error_t::kNoError;
}



