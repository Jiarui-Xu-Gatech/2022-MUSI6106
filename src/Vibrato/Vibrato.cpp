#include "Vibrato.h"



CVibratoIf::CVibratoIf():
m_bIsInitialized(false),
m_fSampleRate(0),
m_fDelayInSamples(0),
m_iNumChannels(0),
m_fWidthInSamples(0)
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
    
// TODO: Change fModWidthInSec to widthInSamples
Error_t CVibratoIf::init(float fDelayInSec, float fFreqModInHz, int iNumChannels, float fSampleRateInHz, float fModWidthInSec)
{

    // Initialize Vibrato variables
    m_fDelayInSamples = fDelayInSec * fSampleRateInHz;
    m_fSampleRate = fSampleRateInHz;
    m_iNumChannels = iNumChannels;
    m_fWidthInSamples = fModWidthInSec * fSampleRateInHz;
    m_fFreqModInSamps = fFreqModInHz / m_fSampleRate;

    // Initialize LFO
    CLFO::create(pCLFO);
    pCLFO -> init(m_fFreqModInSamps, m_fWidthInSamples, m_fSampleRate);

    // initialize RingBuffer for delayLine
    pDelayLine = new CRingBuffer<float>* [iNumChannels];
    int iDelayLineLength = static_cast<int>(2 + m_fDelayInSamples + m_fWidthInSamples * 2);

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

    for (int i=0; i < m_iNumChannels; i++)
    {
        for (int j=0; j < iNumberOfFrames; j++)
        {
            pDelayLine[i] -> putPostInc(ppfInputBuffer[i][j]);
            float delaySamps = (pCLFO -> process()) + m_fDelayInSamples + 1;
            ppfOutputBuffer[i][j] = pDelayLine[i] -> get(delaySamps);
            pDelayLine[i] -> getPostInc();
        }
    }
    return Error_t::kNoError;
}



