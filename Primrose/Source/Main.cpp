#include <Core.hpp>




//System ---> Subsystem --->  Windows (/SUBSYSTEM:WINDOWS) ---> OK!
//System ---> Subsystem --->  Console (/SUBSYSTEM:CONSOLE) ---> CANT COMPILE!
int WinMain(int wargc, wchar_t* wargv[]) {
    std::unique_ptr<Core> Engine = std::make_unique<Core>();


    return 0;
}


//System ---> Subsystem --->  Windows (/SUBSYSTEM:WINDOWS) ---> CANT COMPILE!
//System ---> Subsystem --->  Console (/SUBSYSTEM:CONSOLE) ---> OK!
//int main(int argc, char* argv[]) {
//
//    return 0;
//}