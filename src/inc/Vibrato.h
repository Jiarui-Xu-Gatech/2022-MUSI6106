#if !defined(__Vibrato_hdr__)
#define __Vibrato_hdr__

#include "ErrorDef.h"
#include "RingBuffer.h"
#include "Synthesis.h"
#include <math.h>
#include "Lfo.h"

class CVibratoIf : protected CLFO
{
public:
    
   
    /*! list of parameters for the comb filters */
    enum VibratoParam_t
    {
        kParamDelay,
        kParamFreqMod,
        kParamWidth,
        kParamSampleRate,
        
        kNumVibratoParams
    };

  
    /*! creates a new vibrato instance
    \param pCVibratoIF pointer to the new class
    \return Error_t
    */
    static Error_t create (CVibratoIf*& pCVibratoIF);
    

    /*! destroys a vibrato instance
    \param pCVibratoIF pointer to the class to be destroyed
    \return Error_t
    */
    static Error_t destroy (CVibratoIf*& pCVibratoIF);
   
    
    /*! initializes a vibrato instance
    
    \param fDelayInSec maximum allowed delay in seconds
    \param fSampleRateInHz sample rate in Hz
    \param iNumChannels number of audio channels
    \return Error_t
    */
    Error_t init (float fDelayInSec, float fFreqModInHz, int iNumChannels, float fSampleRateInHz, float fModWidthInSec);
    
    
    /*! resets the internal variables (requires new call of initCombFilter)
    \return Error_t
    */
    Error_t reset ();

    /*! sets a vibrato parameter
    \param eParam what parameter (see ::VibratoParam_t)
    \param fParamValue value of the parameter
    \return Error_t
    */
    Error_t setParam (VibratoParam_t eParam, float fParamValue);


    /*! return the value of the specified parameter
    \param eParam
    \return float
    */
    float   getParam (VibratoParam_t eParam) const;
    
    /*! processes one block of audio
    \param ppfInputBuffer input buffer [numChannels][iNumberOfFrames]
    \param ppfOutputBuffer output buffer [numChannels][iNumberOfFrames]
    \param iNumberOfFrames buffer length (per channel)
    \return Error_t
    */
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);


private:
    CVibratoIf ();
    virtual ~CVibratoIf ();

    CRingBuffer<float>** pDelayLine;

    CLFO*           pCLFO;

    bool            m_bIsInitialized;   //!< internal bool to check whether the initCombFilter function has been called
    float           m_fSampleRate;      //!< audio sample rate in Hz
    float           m_fDelayInSamples;
    int             m_iNumChannels;
    float           m_fWidthInSamples;
    float           m_fFreqModInSamps;

};


#endif // __Vibrato_hdr__

