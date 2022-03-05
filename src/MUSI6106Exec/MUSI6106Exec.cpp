
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

    // Initializations
    static const int        kBlockSize = 1024;

    std::string             sInputFilePath;
    std::string             sOutputFilePath;

    float                   fDelayInSec,
                            fModWidthInSec,
                            fFreqModInHz,
                            **ppfInputAudioData = nullptr,
                            **ppfOutputAudioData = nullptr;

    CAudioFileIf            *phInputAudioFile = nullptr,
                            *phOutputAudioFile = nullptr;


    CAudioFileIf::FileSpec_t stFileSpec;

    // Initialize Vibrato
    CVibratoIf           *pVibrato = nullptr;


    //============================================================================
    // Parse CL Arguments
    //============================================================================

    if(argc > 1 && argc != 6)
    {
        std::cout << "Usage: " << std::endl;
        std::cout << "<input audio Path>" << std::endl;
        std::cout << "<output audio path>" << std::endl;
        std::cout << "<delay in seconds>" << std::endl;
        std::cout << "<modulation width in seconds>" << std::endl;
        std::cout << "modulation frequency in Hz"<< std::endl;
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = argv[2];
        fDelayInSec = atof(argv[3]);
        fModWidthInSec = atof(argv[4]);
        fFreqModInHz = atof(argv[5]);
    }



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

    // Destroy and clear memory
    if (ppfInputAudioData[0] == nullptr || ppfOutputAudioData[0] == nullptr)
    {
        CAudioFileIf::destroy(phInputAudioFile);
        CAudioFileIf::destroy(phOutputAudioFile);
    }

    //============================================================================
    // Create Vibrato and allocate memory
    //============================================================================

    CVibratoIf::create(pVibrato);
    pVibrato -> init(fDelayInSec, fFreqModInHz,
                     stFileSpec.iNumChannels,
                     stFileSpec.fSampleRateInHz,
                     fModWidthInSec);


    //============================================================================
    // Read, Filter and Write Audio
    //============================================================================


    while (!phInputAudioFile->isEof())
    {
        // set block length variable
        long long int iNumFrames = kBlockSize;

        // read data (iNumOfFrames might be updated!)
        phInputAudioFile->readData(ppfInputAudioData, iNumFrames);
        pVibrato->process(ppfInputAudioData, ppfOutputAudioData, kBlockSize);
        phOutputAudioFile->writeData(ppfOutputAudioData, iNumFrames);
    }

    cout << "Vibrato applied! The output audio is saved at : " << sOutputFilePath << endl;

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
