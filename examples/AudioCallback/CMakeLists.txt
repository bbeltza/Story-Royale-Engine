include(sre)
project(SRE_EXAMPLES_AUDIOCALLBACK)

set(SOURCES audio_callback.c)

srEngine_build(SRE_EXAMPLES_AUDIOCALLBACK audio_callback SOURCES)