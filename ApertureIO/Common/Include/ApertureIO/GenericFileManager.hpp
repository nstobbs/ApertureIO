#pragma once

#include <efsw/efsw.h>
#include <efsw/efsw.hpp>

#include <string>
#include <filesystem>
#include <unordered_map>

namespace Aio
{
/**
 * @class GenericFileManager
 * @brief Manages a folder of file and watches over them for changes.
 *  And rebuilds any objects if they source file has changed.
 */

class Shader;
class Texture;

template <typename T>
class GenericFileListener;

template <typename T>
class GenericFileManager
{
public:
    GenericFileManager();
    GenericFileManager(std::string folderPath);
    ~GenericFileManager();    
    
    void AddFileToWatch(std::filesystem::path folderPath, T* object);
    void RemoveFileFromWatch(std::filesystem::path filePath);
    void RemoveObjectFromWatch(T*object);

private:
    friend class GenericFileListener<T>;

    efsw::FileWatcher* _pWatcher;
    std::unordered_map<std::filesystem::path, efsw::WatchID> _watchingFolders;
    std::unordered_map<std::filesystem::path, T*> _watchingFiles;
    GenericFileListener<T>* _pListener;
};

template <typename T> 
class GenericFileListener : public efsw::FileWatchListener
{
public:
    GenericFileListener(GenericFileManager<T>* manager);
    void handleFileAction( efsw::WatchID watchid, const std::string& dir,
								   const std::string& filename, efsw::Action action,
								   std::string oldFilename) override;
private:
    GenericFileManager<T>* _pGenericManager;
};

typedef GenericFileManager<Shader> ShaderFileManager;
//typedef GenericFileManager<Texture> TextureFileManager;

};