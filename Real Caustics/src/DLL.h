#ifndef _DLL_H_
#define _DLL_H_
#include <iostream>
#define DLLEXPORT extern "C" __declspec(dllexport)

class test_class
{
public:
    int i;
    float b;
    void show()
    {
        std::cout << i << "\n" << b << std::endl;
    }
};
#ifdef  __cplusplus
extern "C"
{
    #endif


    DLLEXPORT int main();
    DLLEXPORT void init(int photons, int closest, float r);
    DLLEXPORT void array_test(int* arr);
    DLLEXPORT void test_alloc();
    DLLEXPORT void class_test(test_class test);
    DLLEXPORT void test_free();
    DLLEXPORT void class_test1(Triangle tri)
    //DLLEXPORT extern int number_of_photons;
    #ifdef  __cplusplus
}
#endif
#endif

