#include "Vibrato.h"



CVibratoIf::CVibratoIf():
m_bIsInitialized(false),
m_fSampleRate(0),
m_fMaxDelayLengthInS(0),
m_iNumChannels(0),
m_fFreqModInHz(0)
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
    

Error_t CVibratoIf::init (float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels)
{
    pCRingBuff = new CRingBuffer<float>* [iNumChannels];
    
    int fMaxDelayLengthInSamples = int(fMaxDelayLengthInS * fSampleRateInHz);
    for (int i=0; i< iNumChannels; i++)
    {
        pCRingBuff[i] = new CRingBuffer<float> (fMaxDelayLengthInSamples);
        pCRingBuff[i] -> reset();
    }
    
    return Error_t::kNoError;
}

    
Error_t CVibratoIf::reset ()
{
    m_fSampleRate = 0;
    m_bIsInitialized= false;
    return Error_t::kNoError;
}

    
Error_t CVibratoIf::setParam (VibratoParam_t eParam, float fParamValue)
{
    if (eParam == VibratoParam_t::kParamDelay)
        CVibratoIf::m_fMaxDelayLengthInS = fParamValue;
    else if (eParam == VibratoParam_t::kParamFreqMod)
        CVibratoIf::m_fFreqModInHz = fParamValue;
    
    return Error_t::kNoError;
}
    
   
float CVibratoIf::getParam (VibratoParam_t eParam) const
{
    if (eParam == VibratoParam_t::kParamDelay)
        return CVibratoIf::m_fMaxDelayLengthInS;
    else if (eParam == VibratoParam_t::kParamFreqMod)
        return CVibratoIf::m_fFreqModInHz;
}
    

Error_t CVibratoIf::process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    /*
     
     x = utils.LinearWrap(self.data)

     outputSamps = len(x) + maxDelaySamps
     y = np.zeros(outputSamps)
     ringBuf = utils.LinearRingBuffer(maxDelaySamps)

     deltaPhi = fmod / self.sampling_rate
     phi = 0

     for i in range(outputSamps):
         s = x[i]
         ringBuf.pushSample(s)
         delaySamps = int((math.sin(2 * math.pi * phi) + 1.001) * maxDelaySamps)
         y[i] = ringBuf.delayedSample(delaySamps) * 0.5

         phi = phi + deltaPhi
         while phi >= 1:
             phi -= 1

     self.data = y
     
     */
    
    float deltaPhi = m_fFreqModInHz / m_fSampleRate;
    float phi = 0;
    float fMaxDelayLengthInSamples = m_fMaxDelayLengthInS * m_fSampleRate;
        
    for (int i=0; i < m_iNumChannels; i++)
    {
        for (int j=0; j < iNumberOfFrames; j++)
        {
            pCRingBuff[i] -> putPostInc(ppfInputBuffer[i][j]);
            float delaySamps = float((sin(2 * M_PI * phi)) * fMaxDelayLengthInSamples);
            ppfOutputBuffer[i][j] = pCRingBuff[i] -> getPostInc(delaySamps);
            
            phi += deltaPhi;
            while (phi >= 1)
                phi -= 1;
            // Change phi to come down
        }
    }
}





