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
        if (this->checkFileAccess(entry.path()) && is_regular_file(entry.path())) {
            mtx.lock();
            ++totalFiles;
            mtx.unlock();
        }
    }
}
void FolderScanner::printLoading() {
    string prs = "\rScanning: [";
    int progress = scannedFiles * 100 / totalFiles;
    int pos = progress * barWidth / 100;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) prs += "=";
        else if (i == pos) prs += ">";
        else prs += " ";
    }
    prs = prs + "] " + to_string(progress) + "%";
    cout << prs;
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
        mtx.lock();
        errorFile << "\nError when hashing: " << filepath.u8string() << " - " << e.what() << endl;
        mtx.unlock();
    }
}

FolderScanner::FolderScanner() {
    errorFile.open("error-log.txt");
}
FolderScanner::~FolderScanner() {
    errorFile.close();
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
                else {        mtx.lock();

                    errorFile << "Drive or directory not accessible: " << drivePath.u8string() << endl;
        mtx.unlock();
                }
            }
            catch (const std::exception& e) {        mtx.lock();

                errorFile << "Error accessing drive: " << drivePath.u8string() << " - " << e.what() << endl;
        mtx.unlock();
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

