#include "ApertureIO/Knob.hpp"
#include "ApertureIO/RenderPass.hpp"

namespace Aio
{
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

template<typename T, KnobType knobType>
void Knob<T, knobType>::toJson(QJsonDocument* inDoc)
{

};

template<typename T, KnobType knobType>
void Knob<T, knobType>::fromJson(QJsonDocument* outDoc)
{

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
        return _knobs.at(name)->type();
    };
    return KnobType::None;
};

IKnob* KnobManager::CreateKnob(const KnobType type, const std::string& name)
{
    if (type != KnobType::None) {
        _knobCount++;
        switch (type) {
            case KnobType::Bool:
                _knobs.emplace(name, std::make_unique<BoolKnob>);

            case KnobType::Int:
                _knobs.emplace(name, std::make_unique<IntKnob>);

            case KnobType::Float:
                _knobs.emplace(name, std::make_unique<FloatKnob>);

            case KnobType::String:
                _knobs.emplace(name, std::make_unique<StringKnob>);

            case KnobType::Vec2:
                _knobs.emplace(name, std::make_unique<Vec2Knob>);

            case KnobType::Vec3:
                _knobs.emplace(name, std::make_unique<Vec3Knob>);

            case KnobType::Vec4:
                _knobs.emplace(name, std::make_unique<Vec4Knob>);

            case KnobType::Mat4:
                _knobs.emplace(name, std::make_unique<Vec4Knob>);
        }
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