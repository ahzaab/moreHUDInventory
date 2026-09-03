#include "PCH.h"

#include "ActorValueList.h"
#include "AHZScaleform.h"
#include "HashUtil.h"

double CAHZScaleform::mRound(double r)
{
    return (r >= 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

CAHZScaleform::CAHZScaleform() :
    m_showBookRead(false),
    m_showBookSkill(false),
    m_showKnownEnchantment{ false },
    m_showPosNegEffects{ false },
    m_enableItemCardResize(false)
{
}

void CAHZScaleform::ExtendItemCard(RE::GFxMovieView* view, RE::GFxValue* object, RE::InventoryEntryData* item)
{
    if (!item || !object || !view || !item->object) {
        return;
    }
    auto* baseForm = item->object;
    if (!baseForm) {
        return;
    }

    RE::GFxValue obj;
    view->CreateObject(&obj);

    m_completionistResponse = std::nullopt;

    if (m_completionistInstalled) {
        if (auto* messageInterface = SKSE::GetMessagingInterface()) {
            CompletionistRequest request{ baseForm->GetFormID() };
            messageInterface->Dispatch(1, &request, sizeof(request), "Completionist");
        }
    }

    const auto formType = baseForm->GetFormType();
    if ((formType == RE::FormType::Armor || formType == RE::FormType::Weapon) && m_showKnownEnchantment) {
        RegisterNumber(&obj, "enchantmentKnown", GetIsKnownEnchantment(item));
        // Add the object to the scaleform function
        object->SetMember("AHZItemCardObj", obj);
    } else if (formType == RE::FormType::Book) {
        if (m_showBookSkill) {
            std::string bookSkill = GetBookSkill(baseForm);
            if (bookSkill.length()) {
                RegisterString(&obj, "bookSkill", bookSkill.c_str());
            }
        }

        // Add the object to the scaleform function
        object->SetMember("AHZItemCardObj", obj);
    } else if (formType == RE::FormType::AlchemyItem) {
        if (m_showPosNegEffects) {
            auto alchItem = DYNAMIC_CAST(baseForm, RE::TESForm, RE::AlchemyItem);
            // Check the extra data for enchantments learned by the player
            if (item->extraLists && alchItem) {
                for (auto& list : *item->extraLists) {
                    auto pExtraDataList = list;

                    // Search extra data for player created poisons
                    if (pExtraDataList) {
                        if (pExtraDataList->HasType(RE::ExtraDataType::kPoison)) {
                            if (RE::ExtraPoison* extraPoison = static_cast<RE::ExtraPoison*>(pExtraDataList->GetByType(RE::ExtraDataType::kPoison))) {
                                alchItem = extraPoison->poison;
                            }
                        }
                    }
                }
            }

            std::uint32_t posEffects = 0;
            std::uint32_t negEffects = 0;
            if (GetAlchemyEffectCounts(alchItem, posEffects, negEffects)) {
                RegisterNumber(&obj, "PosEffects", posEffects);
                RegisterNumber(&obj, "NegEffects", negEffects);
            }
        }
        // Add the object to the scaleform function
        object->SetMember("AHZItemCardObj", obj);
    }

    // Static icons
    const char* name = item->GetDisplayName();
    if (!name) {
        name = baseForm->GetName();
    }
    if (!name) {
        name = "";
    }
    auto itemId = static_cast<std::int32_t>(SKSE::HashUtil::CRC32(name, baseForm->formID & 0x00FFFFFF));
    auto iconName = PapyrusMoreHudIE::GetIconName(itemId);

    if (iconName.length()) {
        RegisterString(object, "AHZItemIcon", iconName.c_str());
    }

    auto customIcons = PapyrusMoreHudIE::GetFormIcons(baseForm->formID);

    if (m_completionistResponse && m_completionistResponse->m_display && m_completionistResponse->m_formID == baseForm->formID) {
        customIcons.emplace_back(m_completionistResponse->m_icontype ? "cmpFound"sv : "cmpNew"sv);
    }
    m_completionistResponse = std::nullopt;

    if (!customIcons.empty()) {
        RE::GFxValue entry;
        RE::GFxValue customIconArray;
        view->CreateArray(std::addressof(customIconArray));
        customIconArray.SetArraySize(static_cast<uint32_t>(customIcons.size()));
        auto idx = 0;
        for (auto& ci : customIcons) {
            entry.SetString(ci);
            customIconArray.SetElement(idx++, entry);
        }
        object->SetMember("AHZCustomIcons", customIconArray);
    }
}

bool CAHZScaleform::isSurvivalMode()
{
    using TESGlobal = RE::TESGlobal;
    const auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
    const auto survival = dobj ? dobj->GetObject<TESGlobal>(RE::DefaultObjectID::kSurvivalModeEnabled) : nullptr;
    return survival && *survival ? (*survival)->value == 1.0F : false;
}

void CAHZScaleform::Initialize()
{
    m_showBookRead = g_ahzConfiguration.GetBooleanValue("General", "bShowBookRead", true);
    m_showBookSkill = g_ahzConfiguration.GetBooleanValue("General", "bShowBookSkill", true);
    m_showKnownEnchantment = g_ahzConfiguration.GetBooleanValue("General", "bShowKnownEnchantment", true);
    m_enableItemCardResize = g_ahzConfiguration.GetBooleanValue("General", "bEnableItemCardResize", true);
    m_showPosNegEffects = g_ahzConfiguration.GetBooleanValue("General", "bShowPosNegEffects", true);
}

bool CAHZScaleform::GetWasBookRead(RE::TESForm* theObject)
{
    if (!theObject)
        return false;

    if (theObject->GetFormType() != RE::FormType::Book)
        return false;

    auto item = DYNAMIC_CAST(theObject, RE::TESForm, RE::TESObjectBOOK);
    if (item && (item->IsRead())) {
        return true;
    } else {
        return false;
    }
}

std::string CAHZScaleform::GetBookSkill(RE::TESForm* form)
{
    std::string desc;
    if (!form) {
        return desc;
    }

    if (form->GetFormType() == RE::FormType::Book) {
        auto item = DYNAMIC_CAST(form, RE::TESForm, RE::TESObjectBOOK);

        if (!item)
            return desc;

        // If this is a spell book, then it is not a skill book
        if ((item->data.flags & RE::OBJ_BOOK::Flag::kTeachesSpell) == RE::OBJ_BOOK::Flag::kTeachesSpell)
            return desc;

        if (((item->data.flags & RE::OBJ_BOOK::Flag::kAdvancesActorValue) == RE::OBJ_BOOK::Flag::kAdvancesActorValue) &&
            item->data.teaches.actorValueToAdvance != RE::ActorValue::kNone) {
            auto avList = SKSE::ActorValueList::GetSingleton();
            if (avList) {
                auto info = avList->GetActorValue(item->data.teaches.actorValueToAdvance);
                if (info) {
                    auto fname = DYNAMIC_CAST(info, RE::ActorValueInfo, RE::TESFullName);
                    if (fname && fname->GetFullName()) {
                        desc.append(fname->GetFullName());
                    }
                }
            }
        }
    }
    return desc;
}

auto CAHZScaleform::MagicDisallowEnchanting(RE::BGSKeywordForm* pKeywords) -> bool
{
    if (pKeywords && pKeywords->keywords) {
        for (uint32_t k = 0; k < pKeywords->numKeywords; k++) {
            if (pKeywords->keywords[k]) {
                auto keyword = pKeywords->GetKeywordAt(k).value_or(nullptr);
                if (keyword) {
                    // Had to add this check because https://www.nexusmods.com/skyrimspecialedition/mods/34175?
                    // sets the editor ID for 'MagicDisallowEnchanting' to null (╯°□°）╯︵ ┻━┻
                    auto        asCstr = keyword->GetFormEditorID();
                    std::string keyWordName = asCstr ? asCstr : "";
                    if (keyWordName == "MagicDisallowEnchanting") {
                        return true;  // Is enchanted, but cannot be enchanted by player
                    }
                }
            }
        }
    }
    return false;
}

uint32_t CAHZScaleform::GetIsKnownEnchantment(RE::InventoryEntryData* item)
{
    if (!item || !item->object) {
        return 0;
    }

    //auto pPC = RE::PlayerCharacter::GetSingleton();
    auto baseForm = item->object;

    if ((baseForm) &&
        (baseForm->GetFormType() == RE::FormType::Weapon ||
            baseForm->GetFormType() == RE::FormType::Armor ||
            baseForm->GetFormType() == RE::FormType::Ammo ||
            baseForm->GetFormType() == RE::FormType::Projectile)) {
        RE::EnchantmentItem* enchantment = nullptr;
        auto                 keyWordForm = baseForm->As<RE::BGSKeywordForm>();
        auto                 enchantable = baseForm->As<RE::TESEnchantableForm>();
        if (baseForm->GetFormType() == RE::FormType::Projectile) {
            enchantable = baseForm->As<RE::TESEnchantableForm>();
            keyWordForm = baseForm->As<RE::BGSKeywordForm>();
        }

        bool wasExtra = false;
        if (enchantable) {  // Check the item for a base enchantment
            enchantment = enchantable->formEnchanting;
        }

        if (item->extraLists) {
            for (auto& list : *item->extraLists) {
                if (!list) {
                    continue;
                }
                auto extraEnchant = static_cast<RE::ExtraEnchantment*>(list->GetByType(RE::ExtraDataType::kEnchantment));
                if (extraEnchant) {
                    wasExtra = true;
                    enchantment = extraEnchant->enchantment;
                }
            }
        }

        if (enchantment) {
            if ((enchantment->formFlags & RE::TESForm::RecordFlags::kKnown) == RE::TESForm::RecordFlags::kKnown) {
                return MagicDisallowEnchanting(enchantment) ? 2 : 1;
            } else if (MagicDisallowEnchanting(enchantment)) {
                return 2;
            }

            auto baseEnchantment = static_cast<RE::EnchantmentItem*>(enchantment->data.baseEnchantment);
            if (baseEnchantment) {
                if ((baseEnchantment->formFlags & RE::TESForm::RecordFlags::kKnown) == RE::TESForm::RecordFlags::kKnown) {
                    return MagicDisallowEnchanting(baseEnchantment) ? 2 : 1;
                } else if (MagicDisallowEnchanting(baseEnchantment)) {
                    return 2;
                }
            }
        }

        // Its safe to assume that if it not a base enchanted item, that it was enchanted by the player and therefore, they
        // know the enchantment
        if (wasExtra) {
            return 1;
        } else if (enchantable) {
            return MagicDisallowEnchanting(keyWordForm) ? 2 : 0;
        }
    }
    return 0;
}

void CAHZScaleform::ReplaceStringInPlace(std::string& subject, const std::string& search,
    const std::string& replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
};

void CAHZScaleform::RegisterString(RE::GFxValue* dst, const char* name, const char* str)
{
    if (!dst || !name || !str) {
        return;
    }

    RE::GFxValue fxValue;
    fxValue.SetString(str);
    dst->SetMember(name, fxValue);
};

void CAHZScaleform::RegisterNumber(RE::GFxValue* dst, const char* name, double value)
{
    if (!dst || !name) {
        return;
    }

    RE::GFxValue fxValue;
    fxValue.SetNumber(value);
    dst->SetMember(name, fxValue);
};

void CAHZScaleform::RegisterBoolean(RE::GFxValue* dst, const char* name, bool value)
{
    if (!dst || !name) {
        return;
    }

    RE::GFxValue fxValue;
    fxValue.SetBoolean(value);
    dst->SetMember(name, fxValue);
};
namespace Scaleform
{
    void RegisterListener()
    {
        if (GetModuleHandle(L"Completionist")) {
            auto* messageInterface = SKSE::GetMessagingInterface();
            if (!messageInterface) {
                logger::warn("Completionist is installed, but the SKSE messaging interface is unavailable"sv);
                return;
            }

            CAHZScaleform::Singleton().m_completionistInstalled = true;
            logger::info("Completionist is installed, registering listener"sv);
            messageInterface->RegisterListener("Completionist", [](SKSE::MessagingInterface::Message* a_msg) {
                if (!a_msg || a_msg->type != 2 || !a_msg->data || a_msg->dataLen < sizeof(CompletionistResponse)) {
                    return;
                }
                CAHZScaleform::Singleton().m_completionistResponse = *static_cast<CompletionistResponse*>(a_msg->data);
            });
        }
    }
}

bool CAHZScaleform::GetAlchemyEffectCounts(RE::AlchemyItem* a_alchemyItem, std::uint32_t& a_posEffects, std::uint32_t& a_negEffects)
{
    a_posEffects = 0;
    a_negEffects = 0;

    if (!m_showPosNegEffects || !a_alchemyItem || a_alchemyItem->effects.empty()) {
        return false;
    }

    const bool survivalMode = isSurvivalMode();
    for (auto& effect : a_alchemyItem->effects) {
        if (!effect || !effect->baseEffect) {
            continue;
        }

        const char*       description = effect->baseEffect->magicItemDescription.c_str();
        const std::string effectDescription = description ? description : "";
        if (!survivalMode && effectDescription.find("[SURV=") != std::string::npos) {
            continue;
        }

        if (effect->baseEffect->data.flags.any(
                RE::EffectSetting::EffectSettingData::Flag::kDetrimental,
                RE::EffectSetting::EffectSettingData::Flag::kHostile)) {
            ++a_negEffects;
        } else {
            ++a_posEffects;
        }
    }

    return true;
}

bool CAHZScaleform::GetCurrentCraftingResult(CraftingResultData& a_result)
{
    a_result = {};

    auto snapshotResult = [this, &a_result](RE::TESForm* a_form, bool a_isAlchemyMenu) -> bool {
        if (!a_form) {
            return false;
        }

        const auto formID = a_form->GetFormID();
        if (!formID) {
            return false;
        }

        auto* registeredForm = RE::TESForm::LookupByID(formID);
        if (!registeredForm || registeredForm != a_form) {
            return false;
        }

        a_result.formID = formID;
        a_result.isDynamicForm = a_form->IsDynamicForm();
        a_result.isAlchemyMenu = a_isAlchemyMenu;

        const char* resultName = a_form->GetName();
        a_result.formName = resultName ? resultName : "";

        if (a_isAlchemyMenu) {
            auto* alchemyItem = a_form->As<RE::AlchemyItem>();
            if (alchemyItem) {
                GetAlchemyEffectCounts(alchemyItem, a_result.posEffects, a_result.negEffects);
            }
        }

        return true;
    };

    auto* ui = RE::UI::GetSingleton();
    if (!ui) {
        return false;
    }

    auto craftingMenu = ui->GetMenu<RE::CraftingMenu>();
    if (!craftingMenu) {
        return false;
    }

    auto* craftingSubMenu = craftingMenu->GetCraftingSubMenu();
    if (!craftingSubMenu) {
        return false;
    }

    if (auto* alchemyMenu = skyrim_cast<RE::CraftingSubMenus::CraftingSubMenus::AlchemyMenu*>(craftingSubMenu)) {
        a_result.isAlchemyMenu = true;
        auto* resultPotion = alchemyMenu->resultPotion;
        if (!resultPotion || resultPotion == alchemyMenu->unknownPotion) {
            return false;
        }

        return snapshotResult(resultPotion, true);
    }

    // Smithing, enchanting, and constructible-object menus expose copied formId
    // and text values in their GFx list rows. Use those values in AS2 rather than
    // touching transient native recipe or InventoryEntryData storage here.
    return false;
}
