/* NOTE: If we want to go down this path that all of the
data generated and loaded for the RenderEngine are coming
from RenderPasses. Then we should really go down the path
of changing RenderPasses to being Ops. What would be task
that we can preform on the Device. */
#pragma once
#include "ApertureIO/RenderPass.hpp"
#include <glm/glm.hpp>

namespace Aio
{

class Camera
{
public:
    ~Camera() = default;
    Camera();
    Camera(glm::vec3 position, glm::vec3 lookAt, float aspect);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

    void SetPosition(glm::vec3 position);
    void SetLookAt(glm::vec3 position);
    void SetAspectRatio(float aspectRatio);
    void SetFOV(float angle);

    /* For Debug*/
    void rotateCamera(); 

private:
    void buildCamera();

    glm::vec3 _position = {glm::vec3(1.0f, 1.0f, 1.0f)};
    glm::vec3 _lookAt = {glm::vec3(0.0f, 0.0f, 0.0f)};
    float _aspect = {1.0f};
    float _FOV = {90.0f};

    glm::mat4 _viewMatrix;
    glm::mat4 _projMatrix;    
};

/* For Uploading to
the GPU */
struct CameraData
{
    glm::mat4 view;
    glm::mat4 proj;
};

class CameraManager : public RenderPass
{
public:
    CameraManager();
    void BuildKnobs() override;
    void OnKnobChange(KnobGeneric* knob) override;
    void AllocateResources(RenderEngine*  renderEngine) override; /* Allocated Required Resources */
    void BindResources(RenderEngine* renderEngine) override; /* Bind Resources to the RenderContext */
    void Execute(RenderEngine* renderEngine) override; /* Sumbits the Pass for Rendering */

    void SetActiveCamera(const std::string& name);

    void AddCamera(const std::string& name, UniquePtr<Camera> cam);
    void DeleteCamera(const std::string& name);
    Camera* GetCamera(const std::string& name);
    Camera* GetActiveCamera();

private:
    /* Knob */
    StringKnob* _activeCameraKnob;

    bool doesCameraExist(const std::string& name);
    std::unordered_map<std::string, UniquePtr<Camera>> _cameras;
    std::string _active = {"None"};
    std::string _bufferName = {"CameraBuffer"};
};

};
