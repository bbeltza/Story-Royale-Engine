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

"Extras/AudioFormat.c"
"Extras/ExtraSDL.c"
"Extras/ExtraString.c"
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
)

set(LAYER_SOURCES
"Entity.cpp"
"GuiLayer.cpp"
"World.cpp"
"CameraController.cpp"

"Presets/Gui/GuiButton.cpp"

"Presets/Camera/DragControl.cpp"
"Presets/Camera/EntityControl.cpp"
)
set(COMPONENT_SOURCES
"Gui/AspectRatio.cpp"
"Gui/Modulate.cpp"
"Gui/Padding.cpp"
"Gui/Image.cpp"
"Gui/Text.cpp"
"Gui/List.cpp"
"Gui/Rendering.cpp"

"World/Shape.cpp"
"World/Velocity.cpp"
"World/Sprite.cpp"
)

set(UTIL_SOURCES
    "logging.c"
)

set_dst_src(Components COMPONENT_SOURCES LAYER_SOURCES)

set_src(Base BASE_SOURCES)
set_src(Datatypes DATATYPES_SOURCES)
set_src(GameLayer LAYER_SOURCES)
set_src(utils UTIL_SOURCES)

add_library(StoryRoyaleEngine STATIC ${SOURCES})

set(CORE_SOURCES
"Audio.cpp"
"Containers.cpp"
"DestroyQueue.cpp"
"Input.cpp"
"Querying.cpp"
"Renderer.cpp"
"WindowData.cpp"

"InitEngine.c"
"Runtime.c"
"WindowCreation.c"
)

set_dst_src(Core CORE_SOURCES SRCORE_SOURCES)

add_library(StoryRoyaleEngineCore STATIC ${SRCORE_SOURCES} "Base/Timer.cpp")
target_link_libraries(StoryRoyaleEngineCore StoryRoyaleEngine)
target_compile_definitions(StoryRoyaleEngineCore PRIVATE INTERNAL_DEF=1)

add_subdirectory("os")