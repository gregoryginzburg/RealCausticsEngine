#ifndef _DLL_H_
#define _DLL_H_
#define DLLEXPORT extern "C" __declspec(dllexport)

#ifdef  __cplusplus
extern "C"
{
    #endif


    DLLEXPORT int main();
    DLLEXPORT void init(int photons, int closest, float r);
    //DLLEXPORT extern int number_of_photons;
    #ifdef  __cplusplus
}
#endif
#endif

