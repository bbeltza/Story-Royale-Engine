#pragma once

class EngineClass;

#define ENGINE_BASE friend class EngineClass; friend int main(void); EngineClass* const m_Engine;