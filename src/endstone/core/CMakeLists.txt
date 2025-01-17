cmake_minimum_required(VERSION 3.15)
project(endstone_core LANGUAGES CXX)

find_package(base64 REQUIRED)
find_package(Boost REQUIRED)
find_package(concurrentqueue REQUIRED)
find_package(cpptrace REQUIRED)
find_package(date REQUIRED)
find_package(EnTT REQUIRED)
find_package(magic_enum REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)
find_package(sentry REQUIRED)
find_package(spdlog REQUIRED)

add_library(endstone_core
        crash_handler.cpp
        logger_factory.cpp
        message.cpp
        platform_linux.cpp
        platform_windows.cpp
        player.cpp
        server.cpp
        signal_handler.cpp
        actor/actor.cpp
        actor/mob.cpp
        ban/ip_ban_list.cpp
        ban/player_ban_list.cpp
        block/block.cpp
        block/block_data.cpp
        block/block_face.cpp
        block/block_state.cpp
        boss/boss_bar.cpp
        command/command_lexer.cpp
        command/command_map.cpp
        command/command_origin_wrapper.cpp
        command/command_sender.cpp
        command/command_usage_parser.cpp
        command/command_wrapper.cpp
        command/console_command_sender.cpp
        command/minecraft_command.cpp
        command/minecraft_command_adapter.cpp
        command/proxied_command_sender.cpp
        command/server_command_sender.cpp
        command/defaults/ban_command.cpp
        command/defaults/ban_ip_command.cpp
        command/defaults/ban_list_command.cpp
        command/defaults/pardon_command.cpp
        command/defaults/pardon_ip_command.cpp
        command/defaults/plugins_command.cpp
        command/defaults/reload_command.cpp
        command/defaults/status_command.cpp
        command/defaults/version_command.cpp
        event/handlers/scripting_event_handler.cpp
        event/server/server_list_ping_event.cpp
        form/form_codec.cpp
        inventory/inventory.cpp
        inventory/item_stack.cpp
        inventory/player_inventory.cpp
        lang/language.cpp
        level/dimension.cpp
        level/level.cpp
        network/packet_adapter.cpp
        network/packet_codec.cpp
        network/spawn_particle_effect_packet_codec.cpp
        packs/endstone_pack_source.cpp
        permissions/default_permissions.cpp
        permissions/permissible_base.cpp
        plugin/cpp_plugin_loader.cpp
        plugin/plugin_manager.cpp
        plugin/python_plugin_loader.cpp
        scheduler/async_task.cpp
        scheduler/scheduler.cpp
        scheduler/task.cpp
        scheduler/thread_pool_executor.cpp
        scoreboard/criteria.cpp
        scoreboard/objective.cpp
        scoreboard/score.cpp
        scoreboard/scoreboard.cpp
        scoreboard/scoreboard_packet_sender.cpp
        spdlog/console_log_sink.cpp
        spdlog/file_log_sink.cpp
        spdlog/level_formatter.cpp
        spdlog/spdlog_adapter.cpp
        spdlog/text_formatter.cpp
        util/error.cpp
        util/uuid.cpp
)
add_library(endstone::core ALIAS endstone_core)
target_include_directories(endstone_core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../)
target_link_libraries(endstone_core PUBLIC
        bedrock::bedrock
        endstone::endstone
        aklomp::base64
        boost::boost
        concurrentqueue::concurrentqueue
        cpptrace::cpptrace
        date::date
        EnTT::EnTT
        magic_enum::magic_enum
        nlohmann_json::nlohmann_json
        pybind11::embed
        sentry::sentry
        spdlog::spdlog)
target_compile_definitions(endstone_core PUBLIC PYBIND11_DETAILED_ERROR_MESSAGES)
if (ENDSTONE_ENABLE_DEVTOOLS)
    add_subdirectory(devtools)
    target_link_libraries(endstone_core PRIVATE endstone::devtools)
    target_compile_definitions(endstone_core PUBLIC ENDSTONE_WITH_DEVTOOLS)
endif ()
if (UNIX)
    target_link_libraries(endstone_core PUBLIC ${CMAKE_DL_LIBS})
    target_compile_definitions(endstone_core PUBLIC ENDSTONE_DISABLE_DEVTOOLS)
endif ()
target_compile_definitions(endstone_core PUBLIC ENDSTONE_VERSION="${ENDSTONE_VERSION}")

# The crashpad handler executable must be shipped alongside our shared library
add_custom_target(
        copy_crashpad_handler
        ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${sentry_INCLUDE_DIRS}/../bin/ ${CMAKE_BINARY_DIR}/crashpad
)
add_dependencies(copy_crashpad_handler endstone_runtime)
# Reduce the size of the executable by executing strip if it is present on the system
find_program(STRIP_EXECUTABLE strip)
if (STRIP_EXECUTABLE)
    add_custom_target(
            strip_crashpad_handler
            ALL
            COMMAND ${STRIP_EXECUTABLE} ${CMAKE_BINARY_DIR}/crashpad/crashpad_handler${CMAKE_EXECUTABLE_SUFFIX}
            COMMENT "Stripping crashpad_handler executable for size reduction"
    )
    add_dependencies(strip_crashpad_handler copy_crashpad_handler)
endif ()

include(GNUInstallDirs)
install(TARGETS endstone_core
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${CMAKE_BINARY_DIR}/crashpad/ DESTINATION "endstone/_internal/" COMPONENT endstone_wheel OPTIONAL)