#pragma once

#include "ApertureIO/Pointers.hpp"

#include <glm/glm.hpp>
#include <QJsonDocument>

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>

namespace Aio
{

/* Knob ID */
using KnobID = uint32_t;

/* Knob Types */
// This order has to match the order of the
// variant knob!
enum class KnobType
{
    None = -1,
    Bool = 0,
    Int = 1,
    Float = 2,
    String = 3,
    Vec2 = 4,
    Vec3 = 5,
    Vec4 = 6,
    Mat4 = 7
};

std::string to_string(KnobType type);
KnobType to_KnobType(const std::string& knobTypeStr);

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

/* Knob */
template<typename T, KnobType knobType>
class Knob
{
public:
    Knob() = default;
    ~Knob() = default;
    T GetValue();
    void SetValue(T value);

    KnobType GetType() { return _info.type; }
    std::string GetName() { return _info.name; }

    bool IsValid() { return _isValid; }
    void Validate() { this->_isValid = true; }

    void SetInfo(KnobInfo info);
    KnobInfo GetInfo();

private:
    bool _isValid = {false};
    KnobInfo _info;
    T _value;
};

/* Knob */
template<typename T, KnobType knobType>
void Knob<T, knobType>::SetInfo(KnobInfo info)
{
    _info = info;
};

template<typename T, KnobType knobType>
KnobInfo Knob<T, knobType>::GetInfo()
{
    return _info;
};


template<typename T, KnobType knobType>
T Knob<T, knobType>::GetValue()
{
    return _value;
};

template<typename T, KnobType knobType>
void Knob<T, knobType>::SetValue(T value)
{
    _isValid = false;
    _value = value;
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

// FIXME: Start using ColourKnobs Like RGBA or something!
using ColourKnob = Vec4Knob; 

using KnobGeneric = std::variant<BoolKnob, IntKnob, FloatKnob, StringKnob, Vec2Knob, Vec3Knob, Vec4Knob, Mat4Knob>;

/* Forward Declares */
class RenderPass;

/* Knob Manager */
class KnobManager
{

public:
    KnobManager(RenderPass* pass);
    ~KnobManager() = default;
    KnobGeneric* GetKnob(const std::string& name);
    std::vector<KnobGeneric*> GetAllKnobs();
    KnobType GetKnobType(const std::string& name);
    KnobGeneric* CreateKnob(KnobType type, const std::string& name);
    size_t CalculateHash();
    void CheckForKnobChange();
    
private:
    RenderPass* _pPass;
    std::unordered_map<std::string, KnobGeneric> _knobs;
    uint32_t _knobCount = {0};
    size_t _hash;
};
};