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

"Extras/ExtraSDL.c"
"Extras/ExtraString.c"
)

set(SYS_SOURCES
"log.c"
"error.c"
"sleep.c"
)

set(CORE_SOURCES
"InitEngine.c"
"WindowCreation.c"
"Audio.cpp"
"AudioInternal.cpp"
"DestroyQueue.cpp"
"Input.cpp"
"InputEvents.cpp"
"Renderer.cpp"
"Rendering.cpp"
"Runtime.cpp"
"Window.cpp"
"WindowData.cpp"

"AudioFormat.c"
"Physics.cpp"
"Querying.cpp"
)

set(DATATYPES_SOURCES
"Color.cpp"
"UDim.cpp"
)

set(CLASS_SOURCES
"File/Class.cpp"
"File/Loading.cpp"

"Action.cpp"
"Font.cpp"
"FontRenderer.cpp"
"TargetTexture.cpp"
"Texture.cpp"
"Thread.cpp"
"Timer.cpp"
"Tween.cpp"
"Signal.cpp"
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

set_dst_src(Components COMPONENT_SOURCES LAYER_SOURCES)

set(WINDOWS_SOURCES
##"Windows.cpp"
)

if(WIN32)
    set(SYSTEMDEP_SOURCES ${WINDOWS_SOURCES})
endif()

set_src(Sys SYS_SOURCES)
set_src(Core CORE_SOURCES)
set_src(Classes CLASS_SOURCES)
set_src(Datatypes DATATYPES_SOURCES)
set_src(GameLayer LAYER_SOURCES)
set_src(SystemDependent SYSTEMDEP_SOURCES)

add_library(StoryRoyaleEngine ${SOURCES})