#include "ApertureIO/GenericFileManager.hpp"
#include "ApertureIO/Logger.hpp"
#include "ApertureIO/Shader.hpp"
#include "ApertureIO/Texture.hpp"

namespace Aio 
{
/* GenericFileListener<T> Functions */
template <typename T>
GenericFileListener<T>::GenericFileListener(GenericFileManager<T>* manager)
{
    _pGenericManager = manager;
};

template <typename T>
void GenericFileListener<T>::handleFileAction(efsw::WatchID watchid, const std::string& dir,
								   const std::string& filename, efsw::Action action,
								   std::string oldFilename)
{
    Logger::LogInfo("GenericFileListener: handleFileAction()");
    std::filesystem::path filePath = std::filesystem::path(dir + filename);
    if (action == efsw::Action::Modified && _pGenericManager->_watchingFiles.find(filePath) != _pGenericManager->_watchingFiles.end())
    {
        auto logMsg = "GenericFileListener: SourceFile: " + filename + " was modified.";
        Logger::LogInfo(logMsg);
        T* objectPtr = _pGenericManager->_watchingFiles.at(filePath);
        objectPtr->sourceFileModified();
    }
};

/* GenericFileManager<T> Functions */
template <typename T>
GenericFileManager<T>::GenericFileManager()
{
    _pWatcher =  new efsw::FileWatcher();
    _pListener = new GenericFileListener<T>(this);
    _pWatcher->watch();
};

template <typename T>
GenericFileManager<T>::GenericFileManager(std::string folderPath)
{
    _pWatcher =  new efsw::FileWatcher();
    _pListener = new GenericFileListener<T>(this);
    
    efsw::WatchID folderID = _pWatcher->addWatch(folderPath, _pListener);
    auto logMsg = "GenericFileManager: Watching Dir: " + folderPath;
    Logger::LogInfo(logMsg);
    _watchingFolders.emplace(folderPath, folderID);
    _pWatcher->watch();
};

template <typename T>
GenericFileManager<T>::~GenericFileManager()
{
    delete _pWatcher;
    delete _pListener;
};

template <typename T>
void GenericFileManager<T>::AddFileToWatch(std::filesystem::path filePath, T* object)
{
    std::filesystem::path folderPath = filePath.parent_path();
    std::string folderPathString = folderPath.string() + "/";  
    if (_watchingFiles.find(filePath) == _watchingFiles.end() && _watchingFolders.find(folderPathString) == _watchingFolders.end())
    {
        efsw::WatchID folderID = _pWatcher->addWatch(folderPathString, _pListener);
        auto logMsg = "GenericFileManager: Watching Dir: " + folderPathString;
        Logger::LogInfo(logMsg);
        _watchingFolders.emplace(std::filesystem::path(folderPathString), folderID);
        _watchingFiles.emplace(filePath, object);
    } else if (_watchingFiles.find(filePath) == _watchingFiles.end())
    {
        _watchingFiles.emplace(filePath, object);
    }
};

template <typename T>
void GenericFileManager<T>::RemoveFileFromWatch(std::filesystem::path filePath)
{
    _watchingFiles.erase(filePath);
};

template <typename T>
void GenericFileManager<T>::RemoveObjectFromWatch(T* object)
{
    auto filePath = object->GetSourceFilePath();
    _watchingFiles.erase(filePath);
};

template class GenericFileManager<Shader>;
//template class GenericFileManager<Texture>; 

};

