#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "Transforms.h"

// Reload mode every N frames
#define RELOAD_NUMBER 30

class ModeChecker
{
    public:
        ModeChecker(const std::string& path);
        bool update();
    
        // Shouldn't be public but more convenient
        int mode;
        std::vector<std::string> args;

    private:
        std::string modePath;
        int prevTS;
};

class ModeToggler: public CameraOutput
{
    /*
        Reads from a given file, 
        which decides which CameraOutput to instanciate and with what parameters.

        The point is to be able to dynamically change mode / params.    
    */
    public:
        ModeToggler(const std::string& path);
        void createInstance(int mode, std::vector<std::string>& args);
        uchar *nextData();
        ~ModeToggler();
    
    private:
        ModeChecker modeChecker;
        CameraOutput *cameraOutputInstance;
        int i;
};