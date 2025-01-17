cmake_minimum_required(VERSION 3.15)
project(endstone_python LANGUAGES CXX)

find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
find_package(pybind11 REQUIRED)

pybind11_add_module(endstone_python MODULE
        actor.cpp
        ban.cpp
        block.cpp
        boss.cpp
        command.cpp
        endstone_python.cpp
        event.cpp
        form.cpp
        inventory.cpp
        lang.cpp
        level.cpp
        network.cpp
        permissions.cpp
        plugin.cpp
        scheduler.cpp
        scoreboard.cpp
        util.cpp
)
target_compile_definitions(endstone_python PUBLIC PYBIND11_DETAILED_ERROR_MESSAGES)
target_link_libraries(endstone_python PRIVATE endstone::endstone)
if (UNIX AND ENDSTONE_SEPARATE_DEBUG_INFO)
    add_custom_command(
            TARGET endstone_python
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --only-keep-debug "$<TARGET_FILE:endstone_python>" "$<TARGET_FILE_DIR:endstone_python>/endstone_python.dbg"
            COMMAND ${CMAKE_OBJCOPY} --strip-debug "$<TARGET_FILE:endstone_python>"
            COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink="$<TARGET_FILE_DIR:endstone_python>/endstone_python.dbg" "$<TARGET_FILE:endstone_python>"
            COMMENT "Generating debug symbols and stripping binary: $<TARGET_FILE:endstone_python>"
    )
endif ()

include(GNUInstallDirs)
install(TARGETS endstone_python DESTINATION "endstone/_internal/" COMPONENT endstone_wheel OPTIONAL)
if (MSVC)
    install(FILES $<TARGET_PDB_FILE:endstone_python> DESTINATION "endstone/_internal/" COMPONENT endstone_wheel OPTIONAL)
endif ()
if (UNIX AND ENDSTONE_SEPARATE_DEBUG_INFO)
    install(FILES "$<TARGET_FILE_DIR:endstone_python>/endstone_python.dbg" DESTINATION "endstone/_internal/" COMPONENT endstone_wheel OPTIONAL)
endif ()
