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
    };
    return "None";
};

/* KnobManager */
KnobManager::KnobManager(RenderPass* pass)
{
    _pPass = pass;
};

KnobGeneric* KnobManager::GetKnob(const std::string& name)
{
    if (_knobs.find(name) != _knobs.end()) {
        return &_knobs.at(name);
    };
    return nullptr;
};

std::vector<KnobGeneric*> KnobManager::GetAllKnobs()
{
    std::vector<KnobGeneric*> output;
    for (auto knob : _knobs) {
        output.push_back(&knob.second);
    }
    return output;
};

KnobType KnobManager::GetKnobType(const std::string& name)
{
    if (auto knob = GetKnob(name)) {
        KnobType type = static_cast<KnobType>(knob->index());
        return type;
    };
    return KnobType::None;
};

KnobGeneric* KnobManager::CreateKnob(const KnobType type, const std::string& name)
{
    if (type != KnobType::None) {
        _knobCount++;
        switch (type) {
            case KnobType::Bool: {
                KnobGeneric knob = BoolKnob();
                _knobs.emplace(name, knob);
            }
            case KnobType::Int: {
                KnobGeneric knob = IntKnob();
                _knobs.emplace(name, knob);
            }
            case KnobType::Float: {
                KnobGeneric knob = FloatKnob();
                _knobs.emplace(name, knob);
            }
            case KnobType::String: {
                KnobGeneric knob = StringKnob();
                _knobs.emplace(name, knob);
            }
            case KnobType::Vec2: {
                KnobGeneric knob = Vec2Knob();
                _knobs.emplace(name, knob);
            }
            case KnobType::Vec3: {
                KnobGeneric knob = Vec3Knob();
                _knobs.emplace(name, knob);
            }
            case KnobType::Vec4: {
                KnobGeneric knob = Vec4Knob();
                _knobs.emplace(name, knob);
            }
            case KnobType::Mat4: {
                KnobGeneric knob = Mat4Knob();
                _knobs.emplace(name, knob);
            }
        } 
    }
    return GetKnob(name);
};

size_t KnobManager::CalculateHash()
{
    return size_t(0x00);
};

void KnobManager::CheckForKnobChange()
{
    auto& context = _pPass->GetRenderContext();
    for (const auto knob : _knobs) {

        KnobGeneric currentKnob = knob.second;
        bool validState = std::visit([](auto&& thisKnob) {
            return thisKnob.IsValid();
        }, currentKnob);

        if (validState == false) {
            context.PauseRendering();
            _pPass->OnKnobChange(&currentKnob);

            std::visit([](auto&& thisKnob) {
                thisKnob.Validate();
            }, currentKnob);
            context.UnpauseRendering();
        }
    };
};

}