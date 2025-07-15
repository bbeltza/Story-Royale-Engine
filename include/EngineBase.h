#pragma once

class EngineClass;

#define ENGINE_BASE friend class EngineClass; friend int main(void); EngineClass* m_Engine;