#include "FolderScanner.h";

mutex mtx;

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

bool FolderScanner::checkFileAccessShowError(const path& filepath) {
    error_code ec;
    auto fileStatus = status(filepath, ec);

    if (ec) {
        cerr << "Error getting status of file: " << filepath << " - " << ec.message() << endl;
        return false;
    }

    auto permissions = fileStatus.permissions();
    if ((permissions & perms::owner_read) != perms::none ||
        (permissions & perms::group_read) != perms::none ||
        (permissions & perms::others_read) != perms::none) {
        return true;
    }

    return false;
}
bool FolderScanner::checkFileAccess(const path& filepath) {
    error_code ec;
    auto fileStatus = status(filepath, ec);

    if (ec) 
        return false;

    auto permissions = fileStatus.permissions();
    if ((permissions & perms::owner_read) != perms::none ||
        (permissions & perms::group_read) != perms::none ||
        (permissions & perms::others_read) != perms::none) {
        return true;
    }

    return false;
}
void FolderScanner::countFiles(const string& directory) {
    for (const auto& entry : recursive_directory_iterator(directory, directory_options::skip_permission_denied)) {
        if (this->checkFileAccessShowError(entry.path()) && is_regular_file(entry.path())) {
            mtx.lock();
            ++totalFiles;
            mtx.unlock();
        }
    }
}
void FolderScanner::printLoading() {
    cout << "\rScanning: [";
    int progress = scannedFiles * 100 / totalFiles;
    int pos = progress * barWidth / 100;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << progress << "%";
    cout.flush();
}
void FolderScanner::processFile(const path& filepath) {
    try {
        string fileHash;
        future<string> hashFuture = async(md5HashFile, filepath);
        fileHash = hashFuture.get();

        lock_guard<mutex> lock(mtx);
        ++scannedFiles;
        this->printLoading();
        hashMap[fileHash].push_back(filepath);
    }
    catch (const exception& e) {
        cerr << "\nError when hashing: " << filepath.u8string() << " - " << e.what() << endl;
    }
}

void FolderScanner::scan(const string& directory) {
    this->countFiles(directory);
    vector<thread> threads2;
    for (const auto& entry : recursive_directory_iterator(directory, directory_options::skip_permission_denied)) {
        if (this->checkFileAccess(entry.path()) && is_regular_file(entry.path())) {
            threads2.emplace_back(&FolderScanner::processFile, this, entry.path());
        }
    }

    for (auto& t : threads2) {
        if (t.joinable()) {
            t.join();
        }
    }
}
void FolderScanner::scanAllDrivers() {
    wchar_t drives[256];
    DWORD drive_count = GetLogicalDriveStrings(256, drives);
    vector<thread> threads;

    if (drive_count > 0 && drive_count <= MAX_PATH)
    {
        for (wchar_t* drive = drives; *drive; drive += wcslen(drive) + 1) {
            path drivePath(drive);
            try {
                if (exists(drivePath) && is_directory(drivePath)) {
                    cout << setw(10) << drivePath.u8string() << endl;
                    threads.emplace_back(&FolderScanner::scan, this, drivePath.u8string());
                }
                else {
                    cerr << "Drive or directory not accessible: " << drivePath.u8string() << endl;
                }
            }
            catch (const std::exception& e) {
                cerr << "Error accessing drive: " << drivePath.u8string() << " - " << e.what() << endl;
            }
        }

        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }
}
vector<vector<path>> FolderScanner::findAllDuplicate() {
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

