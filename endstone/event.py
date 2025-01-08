from endstone._internal.endstone_python import (
    ActorDeathEvent,
    ActorEvent,
    ActorKnockbackEvent,
    ActorRemoveEvent,
    ActorSpawnEvent,
    ActorTeleportEvent,
    BlockBreakEvent,
    BlockEvent,
    BlockPlaceEvent,
    BroadcastMessageEvent,
    Cancellable,
    Event,
    EventPriority,
    PlayerChatEvent,
    PlayerCommandEvent,
    PlayerDeathEvent,
    PlayerEvent,
    PlayerInteractActorEvent,
    PlayerInteractEvent,
    PlayerJoinEvent,
    PlayerKickEvent,
    PlayerLoginEvent,
    PlayerQuitEvent,
    PlayerTeleportEvent,
    PluginDisableEvent,
    PluginEnableEvent,
    ScriptMessageEvent,
    ServerCommandEvent,
    ServerEvent,
    ServerListPingEvent,
    ServerLoadEvent,
    ThunderChangeEvent,
    WeatherEvent,
    WeatherChangeEvent,
)

__all__ = [
    "event_handler",
    "Event",
    "EventPriority",
    "ActorEvent",
    "ActorDeathEvent",
    "ActorKnockbackEvent",
    "ActorRemoveEvent",
    "ActorSpawnEvent",
    "ActorTeleportEvent",
    "BlockEvent",
    "BlockBreakEvent",
    "BlockPlaceEvent",
    "Cancellable",
    "PlayerEvent",
    "PlayerChatEvent",
    "PlayerCommandEvent",
    "PlayerDeathEvent",
    "PlayerInteractEvent",
    "PlayerInteractActorEvent",
    "PlayerJoinEvent",
    "PlayerKickEvent",
    "PlayerLoginEvent",
    "PlayerQuitEvent",
    "PlayerTeleportEvent",
    "BroadcastMessageEvent",
    "PluginEnableEvent",
    "PluginDisableEvent",
    "ScriptMessageEvent",
    "ServerEvent",
    "ServerCommandEvent",
    "ServerListPingEvent",
    "ServerLoadEvent",
    "ThunderChangeEvent",
    "WeatherEvent",
    "WeatherChangeEvent",
]


def event_handler(func=None, *, priority: EventPriority = EventPriority.NORMAL, ignore_cancelled: bool = False):
    def decorator(f):
        setattr(f, "_is_event_handler", True)
        setattr(f, "_priority", priority)
        setattr(f, "_ignore_cancelled", ignore_cancelled)
        return f

    if func:
        return decorator(func)

    return decorator