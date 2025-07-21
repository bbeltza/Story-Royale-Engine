set(SOURCES 

"main.cpp"
"DefaultGameSettings.cpp"
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

"Component.cpp"
"Entity.cpp"
"GuiLayer.cpp"
"World.cpp"

"Components/Gui/AspectRatio.cpp"
"Components/Gui/UIModulate.cpp"
"Components/Gui/UIPadding.cpp"
"Components/Gui/UIText.cpp"

"Components/World/Shape.cpp"
"Components/World/Velocity.cpp"
"Components/World/Sprite.cpp"

"Presets/Gui/GuiButton.cpp"

)

set(WINDOWS_SOURCES
"Windows.cpp"
)

if(WIN32)
    set(SYSTEMDEP_SOURCES ${WINDOWS_SOURCES})
endif()

foreach(src ${CORE_SOURCES})
set(SOURCES ${SOURCES} "Core/${src}")
endforeach()
foreach(src ${DATATYPES_SOURCES})
set(SOURCES ${SOURCES} "Datatypes/${src}")
endforeach()
foreach(src ${LAYER_SOURCES})
set(SOURCES ${SOURCES} "GameLayer/${src}")
endforeach()
foreach(src ${SYSTEMDEP_SOURCES})
set(SOURCES ${SOURCES} "SystemDependent/${src}")
endforeach()


add_library(StoryRoyaleEngine
${SOURCES}
)