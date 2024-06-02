
#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

#include <Windows.h>
#include <mutex>
#include <thread>
#include <future> 

#include "md5.h"

using namespace std;
using namespace std::filesystem;


//class finddup
//class scan folder 
//class hash (should has thread)


string md5HashFile(const path& filepath) {
    ifstream file(filepath, ios::binary);
    if (!file) {
        throw runtime_error("Can not open file: " + filepath.string());
    }

    MD5 md5;
    char buffer[8192];
    while (file.read(buffer, sizeof(buffer))) {
        md5.add(buffer, file.gcount());
    }
    if (file.gcount() > 0) {
        md5.add(buffer, file.gcount());
    }

    return md5.getHash();
}

mutex mtx;

class FolderScanner {
private:
    long long totalFiles = 0;
    long long scannedFiles = 0;
    int barWidth = 50;
    unordered_map<string, vector<path>> hashMap;
    void countFiles(const string& directory) {
        for (const auto& entry : recursive_directory_iterator(directory, directory_options::skip_permission_denied)) {
            //cout << setfill(' ') << setw(10) << "Loading: " << entry.path().u8string() << endl;

            if (is_regular_file(entry.path())) {
                mtx.lock();
                ++totalFiles;
                mtx.unlock();
            }
        }
    }
    void printLoading() {
        cout << "\rScanning: [";
        int progress = scannedFiles * 100 / totalFiles;
        int pos = progress*barWidth / 100;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) cout << "=";
            else if (i == pos) cout << ">";
            else cout << " ";
        }
        cout << "] " << progress << "%";
        cout.flush();
    }
    void processFile(const path& filepath) {
        try {
            //cout << setfill(' ') << setw(10) << "Loading: " << entry.path().u8string() << endl;
            /*++scannedFiles;
            int progress = scannedFiles * 100 / totalFiles;
            printLoading(progress);*/

            //string fileHash = md5HashFile(entry.path());
            string fileHash;
            future<string> hashFuture = async(md5HashFile, filepath);
            fileHash = hashFuture.get();

            mtx.lock();
            ++scannedFiles;
            this->printLoading();
            hashMap[fileHash].push_back(filepath);
            mtx.unlock();

        }
        catch (const exception& e) {
            cerr << "\nError when hashing: " << filepath.u8string() << " - " << e.what() << endl;
        }
    }
public:
    void scan(const string& directory) {
        this->countFiles(directory);
        vector<thread> threads2;
        for (const auto& entry : recursive_directory_iterator(directory, directory_options::skip_permission_denied)) {
            if (is_regular_file(entry.path())) {
                threads2.emplace_back(&FolderScanner::processFile, this, entry.path());
            }
        }

        for (auto& t : threads2) {
            if (t.joinable()) {
                t.join();
            }
        }
    }
    void scanAllDrivers() {
        wchar_t drives[256];
        DWORD drive_count = GetLogicalDriveStrings(256, drives);
        vector<thread> threads;

        if (drive_count > 0 && drive_count <= MAX_PATH)
        {
            for (wchar_t* drive = drives; *drive; drive += wcslen(drive) + 1) {
                path drivePath(drive);
                if (exists(drivePath) && is_directory(drivePath)) {
                    cout << setw(10) << drivePath.u8string() << endl;
                    //scan(drivePath.u8string());
                    threads.emplace_back(&FolderScanner::scan, this, drivePath.u8string());
                }
            }

            for (auto& t : threads) {
                if (t.joinable()) {
                    t.join();
                }
            }
        }
    }
    vector<vector<path>> findAllDuplicate() {
        if (hashMap.empty()) return vector<vector<path>>();
        vector<vector<path>> dubList;
        for (const auto& [hashContent, paths] : hashMap) {
            if (paths.size() > 1) {
                vector<path> temp;
                for (const auto& _path : paths) {
                    temp.push_back(_path);
                }
                dubList.push_back(temp);
            }
        }
        return dubList;
    }
    
};


class HandleDuplicateFile {
private:
    vector<vector<path>> duplicateList;
    void removeDuplicate(const vector<path>& pathLink, const int& except) {
        for (int i = 0; i < pathLink.size(); ++i) {
            if (i + 1 != except) {
                if (exists(pathLink[i]))
                    if (remove(pathLink[i]))
                        cout << "Deleted " << pathLink[i].u8string() << endl;
            }
        }
    }
public:
    HandleDuplicateFile(const vector<vector<path>>& duplicateList) {
        this->duplicateList = duplicateList;
    }
    void handleAllFile() {
        for (const auto& paths : duplicateList) {
            system("cls");
            cout << "Files with the same content: \n";
            for (int i = 0; i < paths.size(); ++i) {
                cout << "\t" << i + 1 << " " << paths[i].u8string() << endl;
            }
            cout << "\nWhat do you want to do with these duplicate files ?\n\t0: Skip \n   Number: File you want to keep\nYour choice: ";
            int choice2;
            cin >> choice2;
            if (choice2 > 0 && choice2 <= paths.size()) {
                removeDuplicate(paths, choice2);

            }
            cout << "Press Enter to continue.\n";
            cin.ignore();
            cin.ignore();
        }
    }
    void showAllDuplicate() {
        cout << endl;
        for (const auto& paths : duplicateList) {
            cout << "File " << paths[0].u8string() << " has same content with: \n";
            for (int i = 1; i < paths.size(); ++i) {
                cout << "\t" << paths[i].u8string() << endl;
            }
        }
    }
    void exportDupFileList() {
        if (duplicateList.empty()) return;
        ofstream exportFile("dupfilename.txt");
        for (const auto& paths : duplicateList) {
            exportFile << "File " << paths[0].u8string() << " has same content with: \n";
            for (int i = 1; i < paths.size(); ++i) {
                exportFile << "\t" << paths[i].u8string() << endl;
            }
        }
        exportFile.close();
        cout << "File saved at dupfilename.txt\n";
    }
    long long getNumDupFile() {
        long long sum = 0;
        for (const auto& dubs : duplicateList) {
            sum += dubs.size();
        }
        return sum;
    }
    long long getNumDupContent() {
        return duplicateList.size();
    }
};



int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    try {
        int option;
        string folderPath;
        FolderScanner scner;

        cout << "---Find Duplicate File Program---\n";
        cout << "\nMenu option:\n  *  1: Scan all your PC\n  *  2: Select the folder you want to scan \n  *  Other: Exit\n\nYour choice: ";
        cin >> option;

        switch (option) {
        case 1:
            scner.scanAllDrivers();
            break;
        case 2:
            cout << "\nInput folder path: ";
            cin.ignore();
            getline(cin, folderPath);

            if (!exists(folderPath) || !is_directory(folderPath)) {
                cerr << "Wrong address.\n";
                return 1;
            }
            cout << "Loaing...\n";
            scner.scan(folderPath);
            break;
        default:
            cerr << "Exit.\n";
            return 0;
        }
    
        HandleDuplicateFile hdlDupFile = HandleDuplicateFile(scner.findAllDuplicate());
        hdlDupFile.showAllDuplicate();

        cout << "\nFound " << hdlDupFile.getNumDupFile() << " files "<<hdlDupFile.getNumDupContent() << " duplicate content\n";
        if (hdlDupFile.getNumDupFile() > 0) {
            int choice;
            cout << "\t1: Export data to file\n\t2: Halde now\n\t3: Exit \nYour choice: ";
            cin >> choice;
            switch (choice) {
            case 1:
                hdlDupFile.exportDupFileList();
                break;
            case 2:
                hdlDupFile.handleAllFile();
                break;
            case 3:
                cerr << "Exit.\n";
                return 0;
            default:
                cerr << "Invalid choice.\n";
                return 1;
            }
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}

