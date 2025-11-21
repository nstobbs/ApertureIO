#include "ApertureIO/Knob.hpp"
#include "ApertureIO/RenderPass.hpp"

namespace Aio
{

std::string to_string(KnobType type)
{
    switch (type) {
        case KnobType::Bool:
            return "Bool";
        case KnobType::Int:
            return "Int";
        case KnobType::Float:
            return "Float";
        case KnobType::String:
            return "String";
        case KnobType::Vec2:
            return "Vec2";
        case KnobType::Vec3:
            return "Vec3";
        case KnobType::Vec4:
            return "Vec4";
        case KnobType::Mat4:
            return "Mat4";
    }
};

/* Interface Knob */
void IKnob::SetInfo(KnobInfo info)
{
    _info = info;
};

KnobInfo IKnob::GetInfo()
{
    return _info;
};

/* Knob */
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


/* KnobManager */
KnobManager::KnobManager(RenderPass* pass)
{
    _pPass = pass;
};

IKnob* KnobManager::GetKnob(const std::string& name)
{
    if (_knobs.find(name) != _knobs.end()) {
        return _knobs.at(name).get();
    };
    return nullptr;
};

KnobType KnobManager::GetKnobType(const std::string& name)
{
    if (_knobs.find(name) != _knobs.end()) {
        return _knobs.at(name)->GetType();
    };
    return KnobType::None;
};

IKnob* KnobManager::CreateKnob(const KnobType type, const std::string& name)
{
    if (type != KnobType::None) {
        _knobCount++;
        switch (type) {
            case KnobType::Bool:{
                auto knob = std::make_unique<BoolKnob>();
                _knobs.emplace(name, std::move(knob));
            }
                
            /* 
            case KnobType::Int:
                _knobs.emplace(name, std::make_unique<IntKnob>());
                break;
                
            case KnobType::Float:
                _knobs.emplace(name, std::make_unique<FloatKnob>());
                break;

            case KnobType::String:
                _knobs.emplace(name, std::make_unique<StringKnob>());
                break;
                
            case KnobType::Vec2:
                _knobs.emplace(name, std::make_unique<Vec2Knob>());
                break;
                
            case KnobType::Vec3:
                _knobs.emplace(name, std::make_unique<Vec3Knob>());
                break;

            case KnobType::Vec4:
                _knobs.emplace(name, std::make_unique<Vec4Knob>());
                break;

            case KnobType::Mat4:
                _knobs.emplace(name, std::make_unique<Mat4Knob>());
                break;
        */} 
    }
    return GetKnob(name);
};

size_t KnobManager::CalculateHash()
{

};

void KnobManager::CheckForKnobChange()
{
    auto& context = _pPass->GetRenderContext();
    for (const auto& knob : _knobs) {
        IKnob* currentKnob = knob.second.get();
        if (currentKnob->IsValid() == false) {
            context.PauseRendering();
            _pPass->OnKnobChange(currentKnob);
            currentKnob->Validate();
            context.UnpauseRendering();
        }
    };
};

}