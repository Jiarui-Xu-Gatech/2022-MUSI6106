#if !defined(__Vibrato_hdr__)
#define __Vibrato_hdr__

#include "ErrorDef.h"
#include "RingBuffer.h"
#include "Synthesis.h"
#include <math.h>

class CVibratoIf
{
    
public:
    
   
    /*! list of parameters for the comb filters */
    enum VibratoParam_t
    {
        kParamDelay,
        kParamFreqMod,
        
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
    
    \param fMaxDelayLengthInS maximum allowed delay in seconds
    \param fSampleRateInHz sample rate in Hz
    \param iNumChannels number of audio channels
    \return Error_t
    */
    Error_t init (float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels);
    
    
    /*! resets the internal variables (requires new call of initCombFilter)
    \return Error_t
    */
    Error_t reset ();
   
    /*! sets a comb filter parameter
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

protected:

private:
    CVibratoIf ();
    virtual ~CVibratoIf ();

    CRingBuffer<float>** pCRingBuff;
    
    bool            m_bIsInitialized;   //!< internal bool to check whether the initCombFilter function has been called
    float           m_fSampleRate;      //!< audio sample rate in Hz
    float           m_fMaxDelayLengthInS;
    int             m_iNumChannels;
    float           m_fFreqModInHz;


};


#endif // __Vibrato_hdr__

