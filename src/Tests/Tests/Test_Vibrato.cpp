#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include "Vector.h"
#include "Vibrato.h"

#include "gtest/gtest.h"

#include "RingBuffer.h"


namespace vibrato_test {
    void CHECK_ARRAY_CLOSE(float* buffer1, float* buffer2, int iLength, float fTolerance)
    {
        for (int i = 0; i < iLength; i++)
        {
            EXPECT_NEAR(buffer1[i], buffer2[i], fTolerance);
        }
    }

    void RingBufferTest(int iRingBuffLength) {
        CRingBuffer<float>* pCRingBuffer = 0;
        pCRingBuffer = new CRingBuffer<float>(iRingBuffLength);
        for (int i = 0; i < 5; i++) {
            pCRingBuffer->putPostInc(1.F * i);
        }
        for (int i = 5; i < 30; i++) {
            std::cout << "i: " << i << "getNumValuesInBuffer(): " << pCRingBuffer->getNumValuesInBuffer() << std::endl;
            std::cout << "getPostInc(): " << pCRingBuffer->getPostInc() << std::endl;
            pCRingBuffer->putPostInc(1.F * i);
        }

        //test1 index<0
        std::cout << "RingBuffer_test1" << std::endl;
        pCRingBuffer->reset();
        pCRingBuffer->setReadIdx(-1000);
        std::cout << "test_negetive:-1000" << pCRingBuffer->getReadIdx() << std::endl;

        //test2 index>length
        std::cout << "RingBuffer_test2" << std::endl;
        pCRingBuffer->setWriteIdx(iRingBuffLength + 1000);
        std::cout << "test_huge value:iRingBuffLength+1000" << pCRingBuffer->getWriteIdx() << std::endl;

        //test3 overflow
        std::cout << "RingBuffer_test3" << std::endl;
        pCRingBuffer->reset();
        for (int i = 0; i < 25; i++) {
            pCRingBuffer->putPostInc(1.F * i);
        }
        for (int i = 0; i < 24; i++) {
            std::cout << "overflow_test" << pCRingBuffer->getPostInc() << std::endl;
        }

        //test4 read empty
        std::cout << "RingBuffer_test4" << std::endl;
        pCRingBuffer->reset();
        pCRingBuffer->putPostInc(1.F);
        for (int i = 0; i < 20; i++) {
            std::cout<<"empty_test"<< pCRingBuffer->getPostInc() << std::endl;
        }

        //test5 read before write
        std::cout << "RingBuffer_test5" << std::endl;
        pCRingBuffer->reset();
        for (int i = 0; i < 5; i++)
        {
            std::cout <<"read before write"<< pCRingBuffer->getPostInc() << std::endl;
        }

        for (int i = 1; i < 25; i++)
        {
            std::cout <<"for " << i << std::endl;
            std::cout << pCRingBuffer->getNumValuesInBuffer() << " "; // is 5
            std::cout << pCRingBuffer->getPostInc() << std::endl; // is i-5
            pCRingBuffer->putPostInc(1.F * i);
        }

    }

    void ZeroAmplitudeTest(float** ppfInputBuffer,float** ppfOutputBuffer,CVibratoIf* pCVibrato) {
        std::cout << "ZeroAmplitude Test" << std::endl;
        pCVibrato->init(0.01,48000, 1);
        ppfInputBuffer[0] = new float[2048];
        ppfOutputBuffer[0] = new float[2048];
        for (int i = 0; i < 2048; i++)
        {
            ppfInputBuffer[0][i] = sin(i / 100);
        }
        pCVibrato->process(ppfInputBuffer, ppfOutputBuffer, 2048);
        CHECK_ARRAY_CLOSE(ppfInputBuffer[0], ppfOutputBuffer[0] + 480, 2048 - 480, 1e-4);
        delete[] ppfInputBuffer[0];
        delete[] ppfOutputBuffer[0];
        delete[] ppfInputBuffer;
        delete[] ppfOutputBuffer;
        std::cout << "ZeroAmplitudeTest Pass!" << std::endl;
    }

    void DCInputDCOutputTest(float** ppfInputBuffer, float** ppfOutputBuffer, CVibratoIf* pCVibrato) {
        std::cout << "DCInputDCOutput Test" << std::endl;
        pCVibrato->init(0.01, 48000,1);
        pCVibrato->setParam(CVibratoIf::kParamFreqMod, 10);
        pCVibrato->setParam(CVibratoIf::kParamDelay, 0.005);
        ppfInputBuffer[0] = new float[2048];
        ppfOutputBuffer[0] = new float[2048];
        for (int i = 0; i < 2048; i++)
        {
            ppfInputBuffer[0][i] = 1;
        }
        pCVibrato->process(ppfInputBuffer, ppfOutputBuffer, 2048);
        CHECK_ARRAY_CLOSE(ppfInputBuffer[0] + 240, ppfOutputBuffer[0] + 720, 2048 - 720, 1e-4);

        pCVibrato->setParam(CVibratoIf::kParamFreqMod, 20);
        pCVibrato->setParam(CVibratoIf::kParamDelay, 0.002);
        pCVibrato->process(ppfInputBuffer, ppfOutputBuffer, 2048);
        CHECK_ARRAY_CLOSE(ppfInputBuffer[0] + 96, ppfOutputBuffer[0] + 576, 2048 - 576, 1e-4);


        delete[] ppfInputBuffer[0];
        delete[] ppfOutputBuffer[0];
        delete[] ppfInputBuffer;
        delete[] ppfOutputBuffer;
        std::cout << "DCInputDCOutput Pass!" << std::endl;
    }

    void VaryBlockSizeTest(float** ppfInputBuffer, float** ppfOutputBuffer, CVibratoIf* pCVibrato) {
        std::cout << "VaryBlockSize Test" << std::endl;
        pCVibrato->init(0.01, 48000,1);
        pCVibrato->setParam(CVibratoIf::kParamFreqMod, 10);
        pCVibrato->setParam(CVibratoIf::kParamDelay, 0.005);
        ppfInputBuffer[0] = new float[2048];
        ppfOutputBuffer[0] = new float[2048];
        for (int i = 0; i < 2048; i++)
        {
            ppfInputBuffer[0][i] = 1;
        }
        pCVibrato->process(ppfInputBuffer, ppfOutputBuffer, 32);
        for (int i = 32; i < 2048; i <<= 1)
        {
            float* pfInputBuffer_i = ppfInputBuffer[0] + i;
            float* pfOutputBuffer_i = ppfOutputBuffer[0] + i;
            pCVibrato->process(&pfInputBuffer_i, &pfOutputBuffer_i, i);
        }
        CHECK_ARRAY_CLOSE(ppfInputBuffer[0] + 240, ppfOutputBuffer[0] + 720, 2048 - 720, 1e-4);

        pCVibrato->setParam(CVibratoIf::kParamFreqMod, 20);
        pCVibrato->setParam(CVibratoIf::kParamDelay, 0.002);
        pCVibrato->process(ppfInputBuffer, ppfOutputBuffer, 32);
        for (int i = 32; i < 2048; i <<= 1)
        {
            float* pfInputBuffer_i = ppfInputBuffer[0] + i;
            float* pfOutputBuffer_i = ppfOutputBuffer[0] + i;
            pCVibrato->process(&pfInputBuffer_i, &pfOutputBuffer_i, i);
        }
        CHECK_ARRAY_CLOSE(ppfInputBuffer[0] + 96, ppfOutputBuffer[0] + 576, 2048 - 576, 1e-4);

        delete[] ppfInputBuffer[0];
        delete[] ppfOutputBuffer[0];
        delete[] ppfInputBuffer;
        delete[] ppfOutputBuffer;
        std::cout << "VaryBlockSize Pass!" << std::endl;
    }

    void ZeroInputTest(float** ppfInputBuffer, float** ppfOutputBuffer, CVibratoIf* pCVibrato) {
        std::cout << "ZeroInput Test" << std::endl;
        pCVibrato->init(0.01, 48000,1);
        pCVibrato->setParam(CVibratoIf::kParamFreqMod, 10);
        pCVibrato->setParam(CVibratoIf::kParamDelay, 0.005);
        ppfInputBuffer[0] = new float[2048];
        ppfOutputBuffer[0] = new float[2048];
        for (int i = 0; i < 2048; i++)
        {
            ppfInputBuffer[0][i] = 0;
        }
        pCVibrato->process(ppfInputBuffer, ppfOutputBuffer, 2048);
        CHECK_ARRAY_CLOSE(ppfInputBuffer[0] + 240, ppfOutputBuffer[0] + 720, 2048 - 720, 1e-4);

        pCVibrato->setParam(CVibratoIf::kParamFreqMod, 20);
        pCVibrato->setParam(CVibratoIf::kParamDelay, 0.002);
        pCVibrato->process(ppfInputBuffer, ppfOutputBuffer, 2048);
        for (int i = 0; i < 2048; i++)
        {
            ppfInputBuffer[0][i] = 0;
        }
        CHECK_ARRAY_CLOSE(ppfInputBuffer[0] + 96, ppfOutputBuffer[0] + 576, 2048 - 576, 1e-4);

        delete[] ppfInputBuffer[0];
        delete[] ppfOutputBuffer[0];
        delete[] ppfInputBuffer;
        delete[] ppfOutputBuffer;
        std::cout << "ZeroInput Pass!" << std::endl;
    }

    //add test:set parameters
    void SetParaTest(float** ppfInputBuffer, float** ppfOutputBuffer, CVibratoIf* pCVibrato) {
        std::cout << "SetParameters Test" << std::endl;
        EXPECT_EQ(pCVibrato->setParam(CVibratoIf::kParamFreqMod, -1.5f), Error_t::kFunctionInvalidArgsError);
        EXPECT_EQ(pCVibrato->setParam(CVibratoIf::kParamDelay, -0.1f), Error_t::kFunctionInvalidArgsError);
        std::cout << "SetParameters Pass!" << std::endl;
    }



}

#endif //WITH_TESTS
