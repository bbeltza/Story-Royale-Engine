macro(set_dst_src FLDR SRCS DST)
    foreach(src ${${SRCS}})
        set(${DST} ${${DST}} "${FLDR}/${src}")
    endforeach()
endmacro()
macro(set_src FLDR SRCS)
    set_dst_src(${FLDR} ${SRCS} SOURCES)
endmacro()

set(SOURCES 
"main.c"
"cmd/args.c"
)

set(DATATYPES_SOURCES
"definitions.cpp"
)

set(BASE_SOURCES
"file.c"
"resource.c"
"chunk.c"
"rwops.c"
"audiochunk.c"

"action.cpp"
"audio.cpp"
"display.cpp"
"font.cpp"
"fontrenderer.cpp"
"image.cpp"
"runtime.cpp"
"thread.cpp"
"tween.cpp"

"Signal.cpp"
)

set(ECS_SOURCES
    "camera.cpp"
    "scene.cpp"
    "entity.cpp"
)

set(GUI_SOURCES
    "object.cpp"
)

set(PRESET_SOURCES
    "gui/components/ratio.cpp"
    "gui/components/padding.cpp"
    "gui/components/image.cpp"
    "gui/components/text.cpp"
    "gui/components/list.cpp"
    "gui/components/fill.cpp"
    "gui/components/transform.cpp"
    "gui/objects/button.cpp"

    "ecs/components/Shape.cpp"
    "ecs/components/Sprite.cpp"
    "ecs/components/Velocity.cpp"
    "ecs/camera/DragControl.cpp"
    "ecs/camera/EntityControl.cpp"
)

set(UTIL_SOURCES
    "logging.c"
    "operators.cc"
)

set_dst_src(components COMPONENT_SOURCES LAYER_SOURCES)
set_src(base BASE_SOURCES)
set_src(datatypes DATATYPES_SOURCES)
set_src(utils UTIL_SOURCES)

set_src(ecs ECS_SOURCES)
set_src(gui GUI_SOURCES)
set_src(presets PRESET_SOURCES)
set(SOURCES "${SOURCES}" PARENT_SCOPE)

add_subdirectory(core)
set_dst_src(core CORE_SOURCES SRCORE_SOURCES)
set(CORE_SOURCES ${SRCORE_SOURCES} PARENT_SCOPE)
