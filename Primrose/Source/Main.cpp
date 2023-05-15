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
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    
    //FileManagement::Read("Resources/Shaders/Vertex.Shader");
    //FileManagement::CRead("Resources/Shaders/Vertex.Shader");

    try {
        const std::unique_ptr<Core> Engine = std::make_unique<Core>();
        Engine->Run();
    }
    catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
    }
    return 0;
}   

