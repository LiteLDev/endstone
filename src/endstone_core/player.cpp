// Copyright (c) 2024, The Endstone Project. (https://endstone.dev) All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "endstone/detail/player.h"

#include <boost/uuid/string_generator.hpp>
#include <magic_enum/magic_enum.hpp>

#include "bedrock/deps/raknet/rak_peer_interface.h"
#include "bedrock/entity/components/abilities_component.h"
#include "bedrock/entity/components/user_entity_identifier_component.h"
#include "bedrock/network/minecraft_packets.h"
#include "bedrock/network/packet/set_title_packet.h"
#include "bedrock/network/packet/text_packet.h"
#include "bedrock/network/packet/transfer_packet.h"
#include "bedrock/network/packet/update_abilities_packet.h"
#include "bedrock/network/server_network_handler.h"
#include "bedrock/server/commands/command_origin_data.h"
#include "bedrock/server/commands/command_origin_loader.h"
#include "bedrock/world/actor/player/player.h"
#include "bedrock/world/level/game_type.h"
#include "bedrock/world/level/level.h"
#include "endstone/color_format.h"
#include "endstone/detail/base64.h"
#include "endstone/detail/server.h"

namespace endstone::detail {

EndstonePlayer::EndstonePlayer(EndstoneServer &server, ::Player &player)
    : EndstoneMob(server, player), player_(player), perm_(static_cast<Player *>(this)),
      inventory_(std::make_unique<EndstonePlayerInventory>(player.getInventory()))
{
    auto *component = player.tryGetComponent<UserEntityIdentifierComponent>();
    if (!component) {
        throw std::runtime_error("UserEntityIdentifierComponent is not valid");
    }
    uuid_ = component->uuid.toEndstone();

    switch (component->network_id.getType()) {
    case NetworkIdentifier::Type::RakNet: {
        auto *peer = entt::locator<RakNet::RakPeerInterface *>::value();
        auto addr = peer->GetSystemAddressFromGuid(component->network_id.guid);
        component->network_id.sock.sa_stor = addr.address.sa_stor;
    }
    case NetworkIdentifier::Type::Address:
    case NetworkIdentifier::Type::Address6: {
        address_ = {component->network_id.getAddress(), component->network_id.getPort()};
        break;
    }
    case NetworkIdentifier::Type::NetherNet:
    case NetworkIdentifier::Type::Generic:
        throw std::runtime_error("Unsupported type of NetworkIdentifier");
    }

    server_.players_.emplace(uuid_, this);
}

EndstonePlayer::~EndstonePlayer()
{
    server_.players_.erase(uuid_);
}

void EndstonePlayer::sendMessage(const std::string &message) const
{
    auto packet = MinecraftPackets::createPacket(MinecraftPacketIds::Text);
    auto pk = std::static_pointer_cast<TextPacket>(packet);
    pk->type = TextPacketType::Raw;
    pk->message = message;
    getHandle().sendNetworkPacket(*packet);
}

void EndstonePlayer::sendMessage(const Translatable &message) const
{
    auto packet = MinecraftPackets::createPacket(MinecraftPacketIds::Text);
    auto pk = std::static_pointer_cast<TextPacket>(packet);
    pk->type = TextPacketType::Translate;
    pk->message = message.getTranslationKey();
    pk->params = message.getParameters();
    pk->localize = true;
    getHandle().sendNetworkPacket(*packet);
}

void EndstonePlayer::sendErrorMessage(const std::string &message) const
{
    sendMessage(ColorFormat::Red + message);
}

void EndstonePlayer::sendErrorMessage(const Translatable &message) const
{
    sendMessage(message);
}

Server &EndstonePlayer::getServer() const
{
    return EndstoneActor::getServer();
}

std::string EndstonePlayer::getName() const
{
    return EndstoneActor::getName();
}

bool EndstonePlayer::isPermissionSet(std::string name) const
{
    return perm_.isPermissionSet(name);
}

bool EndstonePlayer::isPermissionSet(const Permission &perm) const
{
    return perm_.isPermissionSet(perm);
}

bool EndstonePlayer::hasPermission(std::string name) const
{
    return perm_.hasPermission(name);
}

bool EndstonePlayer::hasPermission(const Permission &perm) const
{
    return perm_.hasPermission(perm);
}

PermissionAttachment *EndstonePlayer::addAttachment(Plugin &plugin, const std::string &name, bool value)
{
    return perm_.addAttachment(plugin, name, value);
}

PermissionAttachment *EndstonePlayer::addAttachment(Plugin &plugin)
{
    return perm_.addAttachment(plugin);
}

bool EndstonePlayer::removeAttachment(PermissionAttachment &attachment)
{
    return perm_.removeAttachment(attachment);
}

void EndstonePlayer::recalculatePermissions()
{
    perm_.recalculatePermissions();
}

std::unordered_set<PermissionAttachmentInfo *> EndstonePlayer::getEffectivePermissions() const
{
    return perm_.getEffectivePermissions();
}

bool EndstonePlayer::isOp() const
{
    return getHandle().getCommandPermissionLevel() != CommandPermissionLevel::Any;
}

void EndstonePlayer::setOp(bool value)
{
    if (value == isOp()) {
        return;
    }

    getHandle().setPermissions(value ? CommandPermissionLevel::Any : CommandPermissionLevel::GameDirectors);
}

std::uint64_t EndstonePlayer::getRuntimeId() const
{
    return EndstoneActor::getRuntimeId();
}

Location EndstonePlayer::getLocation() const
{
    return EndstoneActor::getLocation();
}

Vector<float> EndstonePlayer::getVelocity() const
{
    return EndstoneActor::getVelocity();
}

bool EndstonePlayer::isOnGround() const
{
    return EndstoneActor::isOnGround();
}

bool EndstonePlayer::isInWater() const
{
    return EndstoneActor::isInWater();
}

bool EndstonePlayer::isInLava() const
{
    return EndstoneActor::isInLava();
}

Level &EndstonePlayer::getLevel() const
{
    return EndstoneActor::getLevel();
}

Dimension &EndstonePlayer::getDimension() const
{
    return EndstoneActor::getDimension();
}

bool EndstonePlayer::isGliding() const
{
    return EndstoneMob::isGliding();
}

UUID EndstonePlayer::getUniqueId() const
{
    return uuid_;
}

const SocketAddress &EndstonePlayer::getAddress() const
{
    return address_;
}

void EndstonePlayer::sendPopup(std::string message) const
{
    auto packet = MinecraftPackets::createPacket(MinecraftPacketIds::Text);
    auto pk = std::static_pointer_cast<TextPacket>(packet);
    pk->type = TextPacketType::Popup;
    pk->message = message;
    getHandle().sendNetworkPacket(*packet);
}

void EndstonePlayer::sendTip(std::string message) const
{
    auto packet = MinecraftPackets::createPacket(MinecraftPacketIds::Text);
    auto pk = std::static_pointer_cast<TextPacket>(packet);
    pk->type = TextPacketType::Tip;
    pk->message = message;
    getHandle().sendNetworkPacket(*packet);
}

void EndstonePlayer::kick(std::string message) const
{
    auto *component = getHandle().tryGetComponent<UserEntityIdentifierComponent>();
    server_.getServerNetworkHandler().disconnectClient(component->network_id, component->sub_client_id,
                                                       Connection::DisconnectFailReason::NoReason, message,
                                                       message.empty());
}

void EndstonePlayer::giveExp(int amount)
{
    getHandle().addExperience(amount);
}

void EndstonePlayer::giveExpLevels(int amount)
{
    getHandle().addLevels(amount);
}

float EndstonePlayer::getExpProgress() const
{
    return getHandle().getLevelProgress();
}

void EndstonePlayer::setExpProgress(float progress)
{
    if (progress < 0.0 || progress > 1.0) {
        server_.getLogger().error("Experience progress must be between 0.0 and 1.0 ({})", progress);
        return;
    }
    auto diff = progress - getExpProgress();
    auto xp_for_next_level = static_cast<float>(::Player::getXpNeededForLevelRange(getExpLevel(), getExpLevel() + 1));
    giveExp(static_cast<int>(std::round(diff * xp_for_next_level)));
}

int EndstonePlayer::getExpLevel() const
{
    return getHandle().getPlayerLevel();
}

void EndstonePlayer::setExpLevel(int level)
{
    if (level < 0) {
        server_.getLogger().error("Experience level must not be negative ({})", level);
        return;
    }
    giveExpLevels(level - getExpLevel());
}

int EndstonePlayer::getTotalExp() const
{
    auto progress = getExpProgress();
    auto base = ::Player::getXpNeededForLevelRange(0, getExpLevel());
    auto xp_for_next_level = static_cast<float>(::Player::getXpNeededForLevelRange(getExpLevel(), getExpLevel() + 1));
    return base + static_cast<int>(std::round(progress * xp_for_next_level));
}

bool EndstonePlayer::getAllowFlight() const
{
    return getHandle().getAbilities().getBool(AbilitiesIndex::MayFly);
}

void EndstonePlayer::setAllowFlight(bool flight)
{
    if (isFlying() && !flight) {
        getHandle().getAbilities().setAbility(AbilitiesIndex::Flying, false);
    }

    getHandle().getAbilities().setAbility(AbilitiesIndex::MayFly, flight);
    updateAbilities();
}

bool EndstonePlayer::isFlying() const
{
    return getHandle().isFlying();
}

void EndstonePlayer::setFlying(bool value)
{
    if (!getAllowFlight()) {
        server_.getLogger().error("Player {} is not allowed to fly.", getName());
        return;
    }

    getHandle().getAbilities().setAbility(AbilitiesIndex::Flying, value);
    updateAbilities();
}

float EndstonePlayer::getFlySpeed() const
{
    return getHandle().getAbilities().getFloat(AbilitiesIndex::FlySpeed);
}

void EndstonePlayer::setFlySpeed(float value) const
{
    getHandle().getAbilities().setAbility(AbilitiesIndex::FlySpeed, value);
    updateAbilities();
}

float EndstonePlayer::getWalkSpeed() const
{
    return getHandle().getAbilities().getFloat(AbilitiesIndex::WalkSpeed);
}

void EndstonePlayer::setWalkSpeed(float value) const
{
    getHandle().getAbilities().setAbility(AbilitiesIndex::WalkSpeed, value);
    updateAbilities();
}

void EndstonePlayer::sendTitle(std::string title, std::string subtitle) const
{
    sendTitle(std::move(title), std::move(subtitle), 10, 70, 20);
}

void EndstonePlayer::sendTitle(std::string title, std::string subtitle, int fade_in, int stay, int fade_out) const
{
    {
        auto packet = MinecraftPackets::createPacket(MinecraftPacketIds::SetTitle);
        auto pk = std::static_pointer_cast<SetTitlePacket>(packet);
        pk->type = SetTitlePacket::TitleType::Title;
        pk->title_text = title;
        pk->fade_in_time = fade_in;
        pk->stay_time = stay;
        pk->fade_out_time = fade_out;
        getHandle().sendNetworkPacket(*packet);
    }
    {
        auto packet = MinecraftPackets::createPacket(MinecraftPacketIds::SetTitle);
        auto pk = std::static_pointer_cast<SetTitlePacket>(packet);
        pk->type = SetTitlePacket::TitleType::Subtitle;
        pk->title_text = subtitle;
        pk->fade_in_time = fade_in;
        pk->stay_time = stay;
        pk->fade_out_time = fade_out;
        getHandle().sendNetworkPacket(*packet);
    }
}

void EndstonePlayer::resetTitle() const
{
    auto packet = MinecraftPackets::createPacket(MinecraftPacketIds::SetTitle);
    auto pk = std::static_pointer_cast<SetTitlePacket>(packet);
    pk->type = SetTitlePacket::TitleType::Reset;
    getHandle().sendNetworkPacket(*packet);
}

std::chrono::milliseconds EndstonePlayer::getPing() const
{
    auto *peer = entt::locator<RakNet::RakPeerInterface *>::value();
    auto *component = getHandle().tryGetComponent<UserEntityIdentifierComponent>();
    auto guid = RakNet::AddressOrGUID(component->network_id.guid);
    return std::chrono::milliseconds(peer->GetAveragePing(guid));
}

void EndstonePlayer::updateCommands() const
{
    auto &registry = server_.getMinecraftCommands().getRegistry();
    AvailableCommandsPacket packet = registry.serializeAvailableCommands();

    auto &command_map = server_.getCommandMap();
    for (auto &data : packet.commands) {
        auto name = data.name;
        auto *command = command_map.getCommand(name);
        if (command && command->isRegistered() && command->testPermissionSilently(*static_cast<const Player *>(this))) {
            continue;
        }
        data.command_flag |= (CommandFlag::HiddenFromPlayer | CommandFlag::HiddenFromBlock);
        data.permission_level = CommandPermissionLevel::Internal;
    }

    getHandle().sendNetworkPacket(packet);
}

bool EndstonePlayer::performCommand(std::string command) const
{
    return server_.dispatchCommand(*Player::asPlayer(), command);
}

GameMode EndstonePlayer::getGameMode() const
{
    auto game_type = getHandle().getPlayerGameType();
    switch (game_type) {
    case GameType::Creative:
        return GameMode::Creative;
    case GameType::Adventure:
        return GameMode::Adventure;
    case GameType::Spectator:
        return GameMode::Spectator;
    default:
        break;
    }
    return GameMode::Survival;
}

void EndstonePlayer::setGameMode(GameMode mode)
{
    switch (mode) {
    case GameMode::Survival:
        getHandle().setPlayerGameType(GameType::Survival);
        break;
    case GameMode::Creative:
        getHandle().setPlayerGameType(GameType::Creative);
        break;
    case GameMode::Adventure:
        getHandle().setPlayerGameType(GameType::Adventure);
        break;
    case GameMode::Spectator:
        getHandle().setPlayerGameType(GameType::Spectator);
        break;
    }
}

PlayerInventory &EndstonePlayer::getInventory() const
{
    return *inventory_;
}

std::string EndstonePlayer::getLocale() const
{
    return locale_;
}

std::string EndstonePlayer::getDeviceOS() const
{
    return device_os_;
}

endstone::UUID EndstonePlayer::getDeviceId() const
{
    return device_id_;
}

const Skin &EndstonePlayer::getSkin() const
{
    return skin_;
}

void EndstonePlayer::transfer(std::string address, int port) const
{
    auto packet = MinecraftPackets::createPacket(MinecraftPacketIds::Transfer);
    auto pk = std::static_pointer_cast<TransferPacket>(packet);
    pk->address = std::move(address);
    pk->port = port;
    getHandle().sendNetworkPacket(*packet);
}

void EndstonePlayer::initFromConnectionRequest(
    std::variant<const ::ConnectionRequest *, const ::SubClientConnectionRequest *> request)
{
    std::visit(
        [&](auto &&request) {
            if (auto locale = request->getData("LanguageCode").asString(); !locale.empty()) {
                locale_ = locale;
            }

            if (auto device_os = request->getData("DeviceOS").asInt(); device_os > 0) {
                auto platform = magic_enum::enum_cast<BuildPlatform>(device_os).value_or(BuildPlatform::Unknown);
                device_os_ = magic_enum::enum_name(platform);
            }

            if (auto device_id = request->getData("DeviceId").asString(); !device_id.empty()) {
                boost::uuids::string_generator gen;
                auto boost_uuid = gen(device_id);
                std::copy(std::begin(boost_uuid.data), std::end(boost_uuid.data), std::begin(device_id_.data));
            }

            {
                auto skin_id = request->getData("SkinId").asString();
                auto skin_height = request->getData("SkinImageHeight").asInt();
                auto skin_width = request->getData("SkinImageWidth").asInt();
                auto skin_data = base64_decode(request->getData("SkinData").asString()).value_or("");
                auto cape_id = request->getData("CapeId").asString();
                auto cape_height = request->getData("CapeImageHeight").asInt();
                auto cape_width = request->getData("CapeImageWidth").asInt();
                auto cape_data = base64_decode(request->getData("CapeData").asString()).value_or("");
                skin_ = {skin_id, Skin::ImageData{skin_height, skin_width, skin_data}, cape_id,
                         Skin::ImageData{cape_height, cape_width, cape_data}};
            }
        },
        request);
}

void EndstonePlayer::disconnect()
{
    perm_.clearPermissions();
}

void EndstonePlayer::updateAbilities() const
{
    auto packet = MinecraftPackets::createPacket(MinecraftPacketIds::UpdateAbilitiesPacket);
    std::shared_ptr<UpdateAbilitiesPacket> pk = std::static_pointer_cast<UpdateAbilitiesPacket>(packet);
    pk->data = {getHandle().getOrCreateUniqueID(), getHandle().getAbilities()};
    getHandle().sendNetworkPacket(*packet);
}

::Player &EndstonePlayer::getHandle() const
{
    return player_;
}

}  // namespace endstone::detail
