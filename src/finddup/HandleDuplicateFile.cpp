#include "HandleDuplicateFile.h"


void HandleDuplicateFile::removeDuplicate(const vector<path>& pathLink, const int& except) {
    for (int i = 0; i < pathLink.size(); ++i) {
        if (i + 1 != except) {
            if (exists(pathLink[i]))
                if (remove(pathLink[i]))
                    cout << "Deleted " << pathLink[i].u8string() << endl;
        }
    }
}

HandleDuplicateFile::HandleDuplicateFile(const vector<vector<path>>& duplicateList) {
    this->duplicateList = duplicateList;
}
void HandleDuplicateFile::handleAllFile() {
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
void HandleDuplicateFile::showAllDuplicate() {
    cout << endl;
    for (const auto& paths : duplicateList) {
        cout << "File " << paths[0].u8string() << " has same content with: \n";
        for (int i = 1; i < paths.size(); ++i) {
            cout << "\t" << paths[i].u8string() << endl;
        }
    }
}
void HandleDuplicateFile::exportDupFileList() {
    if (duplicateList.empty()) return;
    ofstream exportFile("duplicate-fil-list.txt");
    for (const auto& paths : duplicateList) {
        exportFile << "File " << paths[0].u8string() << " has same content with: \n";
        for (int i = 1; i < paths.size(); ++i) {
            exportFile << "\t" << paths[i].u8string() << endl;
        }
    }
    exportFile.close();
    cout << "File saved at duplicate-fil-list.txt\n";
}
long long HandleDuplicateFile::getNumDupFile() {
    long long sum = 0;
    for (const auto& dubs : duplicateList) {
        sum += dubs.size();
    }
    return sum;
}
long long HandleDuplicateFile::getNumDupContent() {
    return duplicateList.size();
}


