#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <Windows.h>
#include <future> 

#include "md5.h"

using namespace std;
using namespace filesystem;

class FolderScanner {
private:
    long long totalFiles = 0;
    long long scannedFiles = 0;
    int barWidth = 50;
    unordered_map<string, vector<path>> hashMap;

    bool checkFileAccess(const path& filepath);
    bool checkFileAccess(const path& filepath);
    void countFiles(const string& directory);
    void printLoading();
    void processFile(const path& filepath);

public:
    void scan(const string& directory);
    void scanAllDrivers();
    vector<vector<path>> findAllDuplicate();
};
