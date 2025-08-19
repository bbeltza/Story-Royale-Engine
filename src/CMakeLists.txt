set(SOURCES 

"main.cpp"
"System.cpp"
"File.cpp"
"Music.cpp"
"Sound.cpp"

"Extras/ExtraSDL.c"
"Extras/ExtraString.c"
)

set(CORE_SOURCES 

"Input.cpp"
"InputEvents.cpp"
"InstanceRendering.cpp"
"Physics.cpp"
"Querying.cpp"
"Rendering.cpp"
"Runtime.cpp"
"Timer.cpp"
"Tween.cpp"

)

set(DATATYPES_SOURCES

"Color.c"
"Signal.cpp"
"UDim.cpp"

)

set(LAYER_SOURCES

"Entity.cpp"
"GuiLayer.cpp"
"World.cpp"

"Components/Gui/AspectRatio.cpp"
"Components/Gui/Modulate.cpp"
"Components/Gui/Padding.cpp"
"Components/Gui/Text.cpp"
"Components/Gui/List.cpp"

"Components/World/Shape.cpp"
"Components/World/Velocity.cpp"
"Components/World/Sprite.cpp"
"Components/World/StaticTexture.cpp"

"Presets/Gui/GuiButton.cpp"

)

set(WINDOWS_SOURCES
"Windows.cpp"
)

if(WIN32)
    set(SYSTEMDEP_SOURCES ${WINDOWS_SOURCES})
endif()

macro(set_src FLDR SRCS)
    foreach(src ${${SRCS}})
        set(SOURCES ${SOURCES} "${FLDR}/${src}")
    endforeach()
endmacro()

set_src(Core CORE_SOURCES)
set_src(Datatypes DATATYPES_SOURCES)
set_src(GameLayer LAYER_SOURCES)
set_src(SystemDependent SYSTEMDEP_SOURCES)

add_library(StoryRoyaleEngine ${SOURCES})