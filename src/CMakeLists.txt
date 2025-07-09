set(SOURCES 

"main.cpp"
"DefaultGameSettings.cpp"
"ExtraSDL.c"
"System.c"
"File.cpp"

)

set(CORE_SOURCES 

"Input.cpp"
"InputEvents.cpp"
"InstanceRendering.cpp"
"Physics.cpp"
"Querying.cpp"
"Rendering.cpp"
"Runtime.cpp"

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

foreach(src ${CORE_SOURCES})
set(SOURCES ${SOURCES} "Core/${src}")
endforeach()
foreach(src ${DATATYPES_SOURCES})
set(SOURCES ${SOURCES} "Datatypes/${src}")
endforeach()
foreach(src ${LAYER_SOURCES})
set(SOURCES ${SOURCES} "GameLayer/${src}")
endforeach()

add_library(StoryRoyaleEngine
${SOURCES}
)