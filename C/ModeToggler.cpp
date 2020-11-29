#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <opencv2/core.hpp>
#include "Transforms.h"
#include "ModeToggler.h"

using namespace std;

ModeChecker::ModeChecker(const string& path)
{
    modePath = path;
    prevTS = -1;
}

bool ModeChecker::update(){
    struct stat lastFileStat;

    stat(modePath.c_str(), &lastFileStat);

    if (lastFileStat.st_mtim.tv_sec == prevTS){
        // No changes
        prevTS = lastFileStat.st_mtim.tv_sec;
        return false;
    }

    prevTS = lastFileStat.st_mtim.tv_sec;
    
    // Read data in file
    string buffer;
    ifstream modeFile;

    modeFile.open(modePath, ios::in);
    getline(modeFile, buffer);
    mode = atoi(buffer.c_str());
    args.clear();
    
    while (getline(modeFile, buffer)){
        if (buffer.size() > 0 && buffer[0] != '#'){
            args.push_back(buffer);
        }
    }
    
    modeFile.close();
    return true;
}

ModeToggler::ModeToggler(const string& path):
modeChecker(path), i(0)
{
    modeChecker.update();
    createInstance(modeChecker.mode, modeChecker.args);
}

uchar *ModeToggler::nextData(){
    i++;
    if (i % RELOAD_NUMBER == 0){
        if(modeChecker.update()){
            cout << "Changed mode! " << modeChecker.mode << endl;
            delete cameraOutputInstance;
            createInstance(modeChecker.mode, modeChecker.args);
        }
    }
    return cameraOutputInstance->nextData();
}

void ModeToggler::createInstance(int mode, vector<string>& args){
    CameraOutput *prev = cameraOutputInstance;
    try
    {
        switch (mode)
        {
        case 0:
            cameraOutputInstance = new WebcamCameraOutput();
            break;
        case 1:
            cameraOutputInstance = new StaticVideoCameraOutput(args[0], WIDTH, HEIGHT);
            break;
        case 2:
            cameraOutputInstance = new StaticImgCameraOutput(args[0], WIDTH, HEIGHT);
            break;
        case 3:
            {
                int th1 = atoi(args[0].c_str()), th2 = atoi(args[1].c_str());
                cameraOutputInstance = new CannyCameraOutput(th1, th2);
            }
            break;
        case 4:
            cameraOutputInstance = new MatrixCameraOutput();
            break;
        case 5:
            if (args.size() >= 4){
                cameraOutputInstance = new DeformationMirrorCameraOutput(
                    atof(args[0].c_str()), atoi(args[1].c_str()), atoi(args[2].c_str()), atoi(args[3].c_str()));
            }
            else{
                cameraOutputInstance = new DeformationMirrorCameraOutput(
                    atof(args[0].c_str()), atoi(args[1].c_str()), HEIGHT / 2, WIDTH / 2);
            }
            break;
        default:
            cerr << "Mode not recognized: " << mode << endl;
            break;
        }

    }
    catch(const exception& e)
    {
        cerr << "Failed to create instance of mode " << mode << '\n' << e.what() << '\n';
        cameraOutputInstance = prev;  // TODO does that work?
    }
}

ModeToggler::~ModeToggler(){
    delete cameraOutputInstance;
}
