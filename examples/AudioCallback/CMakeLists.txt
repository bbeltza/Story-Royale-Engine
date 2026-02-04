include(sre)
project(AudioCallback)

set(SOURCES audio_callback.c)

srEngine_build(AudioCallback audio_callback SOURCES)