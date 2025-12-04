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
)

set(DATATYPES_SOURCES
"Color.cpp"
"UDim.cpp"
)

set(BASE_SOURCES
"File/Stream.cpp"
"File/Chunk.cpp"

"Action.cpp"
"Audio.cpp"
"Font.cpp"
"FontRenderer.cpp"
"Input.cpp"
"Physics.cpp"
"Rendering.cpp"
"Runtime.cpp"
"Signal.cpp"
"TargetTexture.cpp"
"Texture.cpp"
"Thread.cpp"
"Tween.cpp"
"Window.cpp"
"Timer.cpp"
)

set(LAYER_SOURCES
"Entity.cpp"
"GuiLayer.cpp"
"World.cpp"
"CameraController.cpp"

"presets/gui/GuiButton.cpp"

"presets/camera/DragControl.cpp"
"presets/camera/EntityControl.cpp"
)
set(COMPONENT_SOURCES
"gui/AspectRatio.cpp"
"gui/Modulate.cpp"
"gui/Padding.cpp"
"gui/Image.cpp"
"gui/Text.cpp"
"gui/List.cpp"
"gui/Rendering.cpp"

"world/Shape.cpp"
"world/Velocity.cpp"
"world/Sprite.cpp"
)

set(UTIL_SOURCES
    "audioformat.c"
    "logging.c"
)

set_dst_src(components COMPONENT_SOURCES LAYER_SOURCES)
set_src(base BASE_SOURCES)
set_src(datatypes DATATYPES_SOURCES)
set_src(gamelayer LAYER_SOURCES)
set_src(utils UTIL_SOURCES)
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
set_dst_src(Core CORE_SOURCES SRCORE_SOURCES)
set(CORE_SOURCES ${SRCORE_SOURCES} PARENT_SCOPE)
