
#include <iostream>
#include <ctime>
#include "MUSI6106Config.h"
#include "AudioFileIf.h"
#include "Vibrato.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{

    static const int kBlockSize = 1024;
    //============================================================================
    // Parse CL Arguments
    //============================================================================

    std::string sInputFilePath = "/Users/apple/Desktop/output1.wav";
    std::string sOutputFilePath = "/Users/apple/Desktop/Vibrato.wav";
    float fDelayInSec = 0.5f;
    float fAmplitude = 0.5f;
    float fFreqModInHz = 8.0f;

//    if(argc > 1 && argc != 6)
//    {
//        std::cout << "Usage: " << std::endl;
//        std::cout << "<input audio Path>" << std::endl;
//        std::cout << "<output audio path>" << std::endl;
//        std::cout << "filter type: <FIR> or <IIR>" << std::endl;
//        std::cout << "<delay in seconds> (>= 0)" << std::endl;
//        std::cout << "<gain> (-1.0 .... 1.0)"<< std::endl;
//        return -1;
//    }
//    else
//    {
//        sInputFilePath = argv[1];
//        sOutputFilePath = argv[2];
//        fDelayInSec = atof(argv[3]);
//        fAmplitude = atof(argv[4]);
//        fFreqModInHz = atof(argv[5]);
//    }

    float                   **ppfInputAudioData = nullptr,
                            **ppfOutputAudioData = nullptr;

    CAudioFileIf            *phInputAudioFile = nullptr,
                            *phOutputAudioFile = nullptr;


    CAudioFileIf::FileSpec_t stFileSpec;

    // Initialize Vibrato
    CVibratoIf           *pVibrato = nullptr;


    //============================================================================
    // open the input wave file
    //============================================================================

    CAudioFileIf::create(phInputAudioFile);
    phInputAudioFile->openFile(sInputFilePath,
                               CAudioFileIf::kFileRead);
    if (!phInputAudioFile->isOpen())
    {
        cout << "Input wave file open error!" << endl;
        CAudioFileIf::destroy(phInputAudioFile);
    }
    phInputAudioFile->getFileSpec(stFileSpec);

    //============================================================================
    // Open the output audio file
    //============================================================================

    CAudioFileIf::create(phOutputAudioFile);
    phOutputAudioFile -> openFile(sOutputFilePath,
                                  CAudioFileIf::kFileWrite,
                                  &stFileSpec);

    if (!phOutputAudioFile->isOpen())
    {
        cout << "Output wave file open error!" << endl;
        CAudioFileIf::destroy(phOutputAudioFile);
    }

    // allocate memory
    ppfInputAudioData = new float*[stFileSpec.iNumChannels];
    ppfOutputAudioData = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        ppfInputAudioData[i] = new float[kBlockSize];
        ppfOutputAudioData[i] = new float[kBlockSize];
    }

    // Destroy and clear memory: Check if this is needed !

    if (ppfInputAudioData[0] == nullptr || ppfOutputAudioData[0] == nullptr)
    {
        CAudioFileIf::destroy(phInputAudioFile);
        CAudioFileIf::destroy(phOutputAudioFile);
    }

    //============================================================================
    // Create CombFilters and allocate memory
    //============================================================================

    CVibratoIf::create(pVibrato);
    pVibrato -> init(.01f, fFreqModInHz,
                     stFileSpec.iNumChannels,
                     stFileSpec.fSampleRateInHz, fAmplitude);


    //============================================================================
    // Read, Filter and Write Audio
    //============================================================================


    while (!phInputAudioFile->isEof())
    {
        // set block length variable
        long long int iNumFrames = kBlockSize;

        // read data (iNumOfFrames might be updated!)
        phInputAudioFile->readData(ppfInputAudioData, iNumFrames);
        pVibrato->process(ppfInputAudioData, ppfOutputAudioData, iNumFrames);
        phOutputAudioFile->writeData(ppfOutputAudioData, iNumFrames);
    }

    cout << "Filtering complete! The output audio is saved at : " << sOutputFilePath << endl;

    //============================================================================
    // Clean up and free memory
    //============================================================================

    CVibratoIf::destroy(pVibrato);
    phInputAudioFile -> closeFile();
    phOutputAudioFile -> closeFile();
    CAudioFileIf::destroy(phInputAudioFile);
    CAudioFileIf::destroy(phOutputAudioFile);
    for(int m = 0; m < stFileSpec.iNumChannels; m++ )
    {
        delete[] ppfInputAudioData[m];
        delete[] ppfOutputAudioData[m];
    }
    delete[] ppfInputAudioData;
    delete[] ppfOutputAudioData;
    ppfInputAudioData = nullptr;
    ppfOutputAudioData = nullptr;

    // all done

    return 0;
}


void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;
}
