#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "System.h"
#include <map>
#include <unordered_map>
#include <filesystem>
#include <string>
#include <functional>

namespace gea
{

///
/// @brief Contains different File Types that can be used to filter or search for certain files.
///
enum class FileType
{
    UNDEFINED,
    FOLDER,
    PNG,
    JPEG,
    WAV,
    OGG,
    MP3,
    OBJ,
    FBX,
    JSON
};

///
/// @brief Used to contain data about a certain file, such as File Path and File Type
///
struct File
{
    FileType mFileType;
    std::string mMimeType;
    uintmax_t mFileSize;
    std::string mFileName;
    std::filesystem::path mFilePath;
    std::filesystem::directory_entry mFileDirectoryEntry;

    bool mIsUsed = false;
};

///
/// @brief The FileSystem class, keeps track of all files used for the game in the editor. Has functions for adding and removing files.
///
class FileSystem : public System
{
    std::unordered_map<std::string, File> mFiles; //Holds all files uploaded to the editor.
    //TODO: FileSystem - Implement a save system that saves important info about these files to a JSON file, so the data can be saved and loaded when exiting and starting the editor.

    std::filesystem::path mCurrentPath; //Used to save the current file path we are exploring or using

    std::map<FileType, std::filesystem::path> mFileIcons;
    std::unordered_map<std::string, FileType> mStringToEnum = {{"UNDEFINED", FileType::UNDEFINED},{"FOLDER", FileType::FOLDER},{"PNG", FileType::PNG},
        {"JPEG", FileType::JPEG},{"WAV", FileType::WAV},{"OGG", FileType::OGG},
        {"MP3", FileType::MP3},{"OBJ", FileType::OBJ},{"FBX", FileType::FBX},
        {"JSON", FileType::JSON},};

    File GetFileDataFromFilePath(const std::filesystem::path &filePath);
    FileType MapStringToFileType(std::string str);

public:
    FileSystem();

    //Adds a new file to the file system. ?Should potentially check that the file isn't already part of the file system?
    bool AddFile(const std::string &fileName, const std::filesystem::path &filePath);

    //Returns file data about a desired file
    File* GetFileData(const std::string & fileName);
    std::vector<File*> GetAllFileData();

    //Tries to remove a desired file. Will give a warning popup if the desired file is being used by other entities or components.
    std::string RemoveFile(const std::string &fileName);

    //Instantly removes a desired file. Will NOT give any warning if the desired file is being used by other entities or components.
    std::string RemoveFileUnsafe(const std::string &fileName, bool calledByRemoveFile = false);

    //Returns the amount of files in the File System
    int FileAmount();

    //Adds all files and folders within the "Content" directory to the File System
    void ReadInFilesFromContentDirectory();

    std::filesystem::path getFileIcon(const FileType& fileType);

    std::function<void()> FilesContentUpdated;
};

} // End of namespace gea

#endif // FILESYSTEM_H
