#pragma once

#include "ApertureIO/Pointers.hpp"

#include <glm/glm.hpp>
#include <QJsonDocument>
#include <string>
#include <unordered_map>

namespace Aio
{

/* Knob ID */
using KnobID = uint32_t;

/* Knob Types */
enum class KnobType
{
    None,
    Bool,
    Int,
    Float,
    String,
    Vec2,
    Vec3,
    Vec4,
    Mat4
};

/* Knob Range */
struct KnobRange
{
    float max = {1.0f};
    float min = {0.0f};
};

/* Knob UI */
struct KnobUI
{
    std::string displayName = {"Unnamed"};
    bool isHidden = {false};
    std::string group = {"nogroup"};
    std::string tooltip = {"No tooltip!"};
};

/* Knob Info */
struct KnobInfo
{
    KnobID id;
    KnobType type;
    std::string name;
    KnobRange range;
    KnobUI ui;
    bool canAnimate = {false};
};

/* Knob Interface */
class IKnob
{
public:
    virtual ~IKnob() = default;
    virtual KnobType type() = 0;
    virtual void toJson(QJsonDocument* inDoc) = 0;
    virtual void fromJson(QJsonDocument* outDoc) = 0;

    bool IsValid() { return _isValid; }
    void Validate() { _isValid = true; }

    void SetInfo(KnobInfo info);
    KnobInfo GetInfo();

protected:
    bool _isValid = {false};
    KnobInfo _info;
};

/* Knob */
template<typename T, KnobType knobType>
class Knob : IKnob
{
public:
    T GetValue();
    void SetValue(T value);

    KnobType type() override { return _info.type; }
    void toJson(QJsonDocument* inDoc) override;
    void fromJson(QJsonDocument* outDoc) override;

private:
    T _value;
};

/* Forward Declares */
class RenderPass;

/* Knob Manager */
class KnobManager
{

friend IKnob;

public:
    KnobManager(RenderPass* pass);
    IKnob* GetKnob(const std::string& name);
    KnobType GetKnobType(const std::string& name);
    IKnob* CreateKnob(KnobType type, const std::string& name);
    size_t CalculateHash();
    void CheckForKnobChange();
private:
    RenderPass* _pPass;
    std::unordered_map<std::string, UniquePtr<IKnob>> _knobs;
    uint32_t _knobCount = {0};
    size_t _hash;
};

/* Defined Knobs */
using BoolKnob = Knob<bool, KnobType::Bool>;
using IntKnob = Knob<int, KnobType::Int>;
using FloatKnob = Knob<float, KnobType::Float>;
using StringKnob = Knob<std::string, KnobType::String>;
using Vec2Knob = Knob<glm::vec2, KnobType::Vec2>;
using Vec3Knob = Knob<glm::vec3, KnobType::Vec3>;
using Vec4Knob = Knob<glm::vec4, KnobType::Vec4>;
using Mat4Knob = Knob<glm::mat4, KnobType::Mat4>;

using ColourKnob = Vec4Knob;
};