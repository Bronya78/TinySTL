#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>

#endif
#include<iostream>
//#include"vector_test.h"
#include"../vector.h"
int main(){
    //using namespace mystl::test;
    // RUN_ALL_TESTS();
    // vector_test::vector_test();
   // using namespace mystl;
    mystl::vector<int> v1;
    v1.push_back(1);
    mystl::vector<int>::iterator it = v1.begin();
    std::cout<<*it<<std::endl;
    return 0;
}

#if defined(_MSC_VER) && defined(_DEBUG)
  _CrtDumpMemoryLeaks();
#endif