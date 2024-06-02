#pragma once
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>

using namespace std;
using namespace filesystem;

class HandleDuplicateFile {
private:
    vector<vector<path>> duplicateList;
    void removeDuplicate(const vector<path>& pathLink, const int& except);

public:
    HandleDuplicateFile(const vector<vector<path>>& duplicateList);
    void handleAllFile();
    void showAllDuplicate();
    void exportDupFileList();
    long long getNumDupFile();
    long long getNumDupContent();
};


