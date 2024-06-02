#include "FolderScanner.h"
#include "HandleDuplicateFile.h"


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
            cout << "Loaing...\n";
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
    
        cout << "\n\nView all errors at error-log.txt";

        HandleDuplicateFile hdlDupFile = HandleDuplicateFile(scner.findAllDuplicate());
        hdlDupFile.showAllDuplicate();

        cout << "\nFound " << hdlDupFile.getNumDupFile() << " files "<<hdlDupFile.getNumDupContent() << " duplicate content\n";
        if (hdlDupFile.getNumDupFile() > 0) {
            int choice;
            cout << "\t1: Export data to file\n\t2: Hanlde now\n\t3: Exit \nYour choice: ";
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

