#include <Core.hpp>
#include <iostream>



//System ---> Subsystem --->  Windows (/SUBSYSTEM:WINDOWS) ---> OK!
//System ---> Subsystem --->  Console (/SUBSYSTEM:CONSOLE) ---> CANT COMPILE!
//int WinMain(int wargc, wchar_t* wargv[]) {
//    std::unique_ptr<Core> Engine = std::make_unique<Core>();
//
//    std::stringstream Shader = FileManagement::ReadFromFile("Resources/Shaders/Vertex.Shader");
//    std::cout << Shader.str();
//    return 0;
//}


//System ---> Subsystem --->  Windows (/SUBSYSTEM:WINDOWS) ---> CANT COMPILE!
//System ---> Subsystem --->  Console (/SUBSYSTEM:CONSOLE) ---> OK!
int main(int argc, char* argv[]) {

    std::string Shader = FileManagement::Read("Resources/Shaders/Vertex.Shader");
    std::cout << Shader;
    return 0;
}   

