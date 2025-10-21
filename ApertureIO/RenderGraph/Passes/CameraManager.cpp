#include "ApertureIO/CameraManager.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Aio
{

/* Camera */
Camera::Camera()
{
    buildCamera();
};

Camera::Camera(glm::vec3 position, glm::vec3 lookAt, float aspect)
{
    _position = position;
    _lookAt = lookAt;
    _aspect = aspect;
    buildCamera();
};

void Camera::buildCamera()
{
    _viewMatrix = glm::lookAt(_position, _lookAt, glm::vec3(0.0f, 0.0f, 1.0f));
    _projMatrix = glm::perspective(glm::radians(_FOV), _aspect, 0.1f, 100.0f);
    _projMatrix[1][1] *= -1.0f;
};

void Camera::rotateCamera()
{
    _viewMatrix = glm::rotate(_viewMatrix, glm::radians(0.01f), glm::vec3(0.0f, 0.0f, 1.0f));
};

glm::mat4 Camera::GetViewMatrix()
{
    return _viewMatrix;
};

glm::mat4 Camera::GetProjectionMatrix()
{
    return _projMatrix;
};

void Camera::SetPosition(glm::vec3 position)
{
    _position = position;
    buildCamera();
};

void Camera::SetLookAt(glm::vec3 position)
{
    _lookAt = position;
    buildCamera();
};

void Camera::SetAspectRatio(float aspectRatio)
{
    _aspect = aspectRatio;
    buildCamera();
};

void Camera::SetFOV(float angle)
{
    _FOV = angle;
    buildCamera();
};

/* Camera Manager */
CameraManager::CameraManager()
{
    /* RenderPass Info */
    _name = "CameraManager";
    _type = RenderPassType::Transfer;

    /* Resources */
    ResourceAccess cameraBufferAccess{};
    cameraBufferAccess.name = _bufferName;
    cameraBufferAccess.type = ResourceType::Uniform;
    cameraBufferAccess.access = AccessType::Write;
    cameraBufferAccess.isInitialisingResource = true;
    _resourcesAccess.push_back(cameraBufferAccess);

    Port cameraOut(this);
    _outPorts.emplace("camera", cameraOut);
};

void CameraManager::AllocateResources(RenderEngine*  renderEngine)
{
    BufferLayout layout;
    BufferElement matrixElement;
    matrixElement.count = 4 * 4;
    matrixElement.normalized = false;
    matrixElement.type = BufferElementType::Float;
    layout.AddBufferElement(matrixElement);
    layout.AddBufferElement(matrixElement);

    BufferCreateInfo cameraBufferInfo{};
    cameraBufferInfo.context = renderEngine->GetContextPtr();
    cameraBufferInfo.device = renderEngine->GetDevicePtr();
    cameraBufferInfo.type = BufferType::Uniform;
    cameraBufferInfo.count = 1;
    cameraBufferInfo.layout = layout;
    renderEngine->StoreBufferPtr(_bufferName, Buffer::CreateBuffer(cameraBufferInfo));
    _outPorts.at("camera").SetOutgoingResource(ResourceType::Uniform, _bufferName);
};

void CameraManager::BindResources(RenderEngine* renderEngine)
{
    /* Blank */
};

void CameraManager::Execute(RenderEngine* renderEngine)
{
    /* Upload Current Camera */
    GetActiveCamera()->rotateCamera();
    CameraData data{};
    data.view = GetActiveCamera()->GetViewMatrix();
    data.proj = GetActiveCamera()->GetProjectionMatrix();
    renderEngine->GetBufferPtr(_bufferName)->UploadToDevice(&data);
};

bool CameraManager::doesCameraExist(const std::string& name)
{
    return _cameras.find(name) != _cameras.end();
}

void CameraManager::SetActiveCamera(const std::string& name)
{
    if (doesCameraExist(name))
    {
        _active = name;
    } else {
        auto warn = "Couldn't find Camera called: " + name + "\n";
        Logger::LogWarn(warn);
    }
};

void CameraManager::AddCamera(const std::string& name, UniquePtr<Camera> cam)
{
    if (!doesCameraExist(name))
    {
        _cameras.emplace(name, std::move(cam));
    }
};

void CameraManager::DeleteCamera(const std::string& name)
{
    if (doesCameraExist(name))
    {
        _cameras.erase(name);
    };
};

Camera* CameraManager::GetCamera(const std::string& name)
{
    if(doesCameraExist(name))
    {
        return _cameras.at(name).get();
    };
    return nullptr;
};

Camera* CameraManager::GetActiveCamera()
{
    if (_active != "None")
    {
        return _cameras.at(_active).get();
    };
    return nullptr;
};

};

