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
"color.cpp"
"UDim.cpp"
)

set(BASE_SOURCES
"File/Stream.cpp"
"File/Chunk.cpp"

"Action.cpp"
"Audio.cpp"
"Font.cpp"
"FontRenderer.cpp"
"Image.cpp"
"Input.cpp"
"Physics.cpp"
"Rendering.cpp"
"Runtime.cpp"
"Signal.cpp"
"Texture.cpp"
"Thread.cpp"
"Tween.cpp"
"Window.cpp"
"Timer.cpp"
)

set(WORLD_SOURCES
    "CameraController.cpp"
    "Entity.cpp"
    "World.cpp"
)

set(GUI_SOURCES
    "GuiLayer.cpp"
    #"layer.cpp"
    #"container.cpp"
)

set(PRESET_SOURCES
    "gui/components/AspectRatio.cpp"
    "gui/components/Modulate.cpp"
    "gui/components/Padding.cpp"
    "gui/components/Image.cpp"
    "gui/components/Text.cpp"
    "gui/components/List.cpp"
    "gui/components/Rendering.cpp"
    "gui/objects/GuiButton.cpp"

    "world/components/Shape.cpp"
    "world/components/Sprite.cpp"
    "world/components/Velocity.cpp"
    "world/camera/DragControl.cpp"
    "world/camera/EntityControl.cpp"
)

set(UTIL_SOURCES
    "audioformat.c"
    "logging.c"
)

set_dst_src(components COMPONENT_SOURCES LAYER_SOURCES)
set_src(base BASE_SOURCES)
set_src(datatypes DATATYPES_SOURCES)
set_src(utils UTIL_SOURCES)

set_src(world WORLD_SOURCES)
set_src(gui GUI_SOURCES)
set_src(presets PRESET_SOURCES)
set(SOURCES "${SOURCES}" PARENT_SCOPE)

set(CORE_SOURCES
"Audio.cpp"
"Containers.cpp"
"DestroyQueue.cpp"
"Input.cpp"
"Querying.cpp"
"Renderer.cpp"

"InitEngine.c"
"Runtime.c"
"Window.c"
)
set_dst_src(core CORE_SOURCES SRCORE_SOURCES)
set(CORE_SOURCES ${SRCORE_SOURCES} PARENT_SCOPE)
