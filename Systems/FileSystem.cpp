#include "FileSystem.h"
#include <QString>
#include "../core.h"
#include "../magic_enum.hpp"

namespace gea
{

/// @brief - Returns a File struct containing data about the file located at the given file path
/// @param filePath - The file path to get data from
/// @return - A File struct
File FileSystem::GetFileDataFromFilePath(const std::filesystem::path &filePath)
{
    std::filesystem::directory_entry entry(filePath);

    uintmax_t fileSize = 0;

    if (entry.is_regular_file()) fileSize = entry.file_size();

    if (entry.is_regular_file())
    {
        return File{MapStringToFileType(filePath.extension().string()), filePath.extension().string(), fileSize, entry.path().filename().string(), entry.path(), entry};
    }

    return {};
}

///
/// @brief FileSystem::MapStringToFileType
/// @param str
/// @return
///
FileType FileSystem::MapStringToFileType(std::string str)
{
    if (!str.empty() && str.front() == '.') str.erase(0,1);

    auto toUpperCase = [](const std::string& input) -> std::string {
        std::string result = input;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return result;
    };

    auto it = mStringToEnum.find(toUpperCase(str));

    if (it != mStringToEnum.end())
    {
        qDebug() << std::string(magic_enum::enum_name(it->second));
        return it->second;
    }
    qDebug() << "Could not find value " << toUpperCase(str) << " in map";
    return (it != mStringToEnum.end()) ? it->second : FileType::UNDEFINED;
}

///
/// @brief Constructor for File System. Initializes variables such as mCurrentPath
///
FileSystem::FileSystem()
{
    mCurrentPath = EngineInit::mEngineContentDirectory.filesystemAbsolutePath();

    std::filesystem::path fileUndefinedPath = gea::EngineInit::mEngineAssetsDirectoryPath / "icon_undefinedFile.png";

    // Lambda function, returns inserted fileName as filePath if valid, if not returns fileUndefinedPath or "nothing"
    auto resolveIconPath = [&](const std::string& filename) -> std::filesystem::path {
        std::filesystem::path targetPath = gea::EngineInit::mEngineAssetsDirectoryPath / filename;
        if (std::filesystem::exists(targetPath)) {
            return targetPath;
        } else if (std::filesystem::exists(fileUndefinedPath)) {
            return fileUndefinedPath;
        } else {
            return "nothing";
        }
    };

    // Emplaces all FileType enum values and their icons into mFileIcons map
    if (!fileUndefinedPath.empty())
    {
        mFileIcons.try_emplace(FileType::UNDEFINED, resolveIconPath("icon_undefinedFile.png"));
        mFileIcons.try_emplace(FileType::FOLDER, resolveIconPath("icon_folder.png"));
        mFileIcons.try_emplace(FileType::PNG, resolveIconPath("icon_image.png"));
        mFileIcons.try_emplace(FileType::JPEG, resolveIconPath("icon_image.png"));
        mFileIcons.try_emplace(FileType::WAV, resolveIconPath("icon_audio.png"));
        mFileIcons.try_emplace(FileType::OGG, resolveIconPath("icon_audio.png"));
        mFileIcons.try_emplace(FileType::MP3, resolveIconPath("icon_audio.png"));
        mFileIcons.try_emplace(FileType::OBJ, resolveIconPath("icon_model.png"));
        mFileIcons.try_emplace(FileType::FBX, resolveIconPath("icon_model.png"));
        mFileIcons.try_emplace(FileType::JSON, resolveIconPath("icon_json.png"));
    }
}

///
/// @brief - Adds a target file to the File System.
/// @param fileName - An arbitrary name for the File. Has to be unique.
/// @param filePath - The file path for the file we want to add.
/// @return - Whether the file was added successfully to the File System.
///
bool FileSystem::AddFile(const std::string &fileName, const std::filesystem::path &filePath)
{
    if (mFiles.find(fileName) != mFiles.end())
    {
        return false; // FILE NAME IS ALREADY USED
    }

    File file = GetFileDataFromFilePath(filePath);

    if (file.mFileDirectoryEntry.exists())
    {
        try
        {
            std::filesystem::copy_file(
                filePath,
                EngineInit::mEngineContentDirectory.filesystemAbsolutePath() / fileName,
                std::filesystem::copy_options::overwrite_existing
                );
        } catch (const std::filesystem::filesystem_error& err)
        {
            qWarning("File copy failed %s", err.what());
        }

        mFiles.insert({fileName, file});
        return true;
    }

    return false; // FILE COULD NOT BE FOUND
}

///
/// @brief - Tries to remove a file from the File System. Will give a warning if the target file is being used within our game / editor.
/// @param fileName - Name of the file to remove.
/// @return - Whether the file was removed successfully.
///
std::string FileSystem::RemoveFile(const std::string &fileName)
{
    //TODO: FileSystem::RemoveFile() - Decide whether the return type should be more expressive than true/false, maybe giving a return message for why the file wasn't removed.

    auto ref = mFiles.find(fileName);

    if (ref == mFiles.end()) return "Warning: FileSystem::RemoveFile() - File '" + fileName + "' was not found, and could therefore not be deleted.";

    if (ref->second.mIsUsed) return "Warning: FileSystem::RemoveFile() - File '" + fileName + "' is in use, and could therefore not be deleted.";

    return RemoveFileUnsafe(fileName, true);
}

///
/// @brief - Forcibly removes a file from the File System. Will NOT give a warning if the target file is being used.
/// @param fileName - Name of the file to remove.
/// @param calledByRemoveFile - Whether this function was called as part of the FileSystem::RemoveFile() function, or directly by itself
/// @return - Whether the file was removed successfully.
///
std::string FileSystem::RemoveFileUnsafe(const std::string &fileName, bool calledByRemoveFile)
{
    //TODO: FileSystem::RemoveFileUnsafe() - Check if deleting of file works as intended

    std::string prefix = "FileSystem::RemoveFile";
    if (!calledByRemoveFile) prefix += "Unsafe";

    auto ref = mFiles.find(fileName);

    if (ref == mFiles.end()) return "Warning: " + prefix + "() - File '" + fileName + "' was not found, and could therefore not be deleted.";

    bool fileDeleteSucceeded = std::filesystem::remove(ref->second.mFilePath);

    bool fileRemoveSucceeded = mFiles.erase(fileName);

    int i = (fileDeleteSucceeded << 1) | fileRemoveSucceeded;

    switch (i)
    {
    case 0:
        return "Warning: " + prefix + "() - Tried to delete '" + fileName + "', but the file could not be found.";
    case 1:
        return "Warning: " + prefix + "() - Failed to delete actual file '" + fileName + "', but the file reference in the File System class was removed successfully.";
    case 2:
        return "Warning: " + prefix + "() - Failed to remove the file reference for '" + fileName + "' in the File System class, but the actual file was deleted successfully.";
    case 3:
        return "Success: " + prefix + "() - File '" + fileName + "' was deleted successfully!";
    default:
        return "Error: " + prefix + "() - An unknown error occured while trying to delete '" + fileName + "'.";
    }
}

///
/// @brief FileSystem::FileAmount
/// @return The amount of files in the File System
///
int FileSystem::FileAmount()
{
    return mFiles.size();
}

///
/// @brief FileSystem::GetFileData
/// @param fileName
/// @return
///
File* FileSystem::GetFileData(const std::string & fileName)
{
    if (mFiles.find(fileName) != mFiles.end()) return &mFiles[fileName];

    return nullptr;
}

///
/// @brief FileSystem::GetAllFileData
/// @return
///
std::vector<File*> FileSystem::GetAllFileData()
{
    std::vector<File*> fileData;
    fileData.reserve(FileAmount());

    for (auto& [key, value] : mFiles)
    {
        fileData.push_back(&value);
    }

    QLog("Returning " + QString::number(fileData.size()) + " files", "grey");

    return fileData;
}

///
/// @brief FileSystem::ReadInFilesFromContentDirectory
///
void FileSystem::ReadInFilesFromContentDirectory()
{
    if (!EngineInit::mEngineContentDirectory.exists()) return;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(EngineInit::mEngineContentDirectory.filesystemAbsolutePath()))
    {
        try
        {
            AddFile(entry.path().filename().string(), entry.path());
        }
        catch (const std::filesystem::filesystem_error& err)
        {
            QLog("Error accessing " + QString::fromStdString(entry.path().string()) + ": " + err.what(), "red");
        }
    }

    if (FilesContentUpdated)
    {
        FilesContentUpdated();
    }
}

///
/// @brief getFileIcon - Returns the file path to a given FileType
/// @param fileType - Key to find corresponding file path in mFileIcons map
/// @return - filePath to icon for given FileType
///
std::filesystem::path FileSystem::getFileIcon(const FileType& fileType)
{
    return (mFileIcons.find(fileType) != mFileIcons.end()) ? mFileIcons.at(fileType) : "nothing";
}

} // End of namespace gea
