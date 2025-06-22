#include <standard.h>

#include "Engine.h"
#include "Window.h"
#include "Input.h"
#include "ECS.h"
#include "GuiLayer.h"
#include "GuiComponents.h"
#include "Components.h"

#include "GameSettings.h"

#define WCENTERED SDL_WINDOWPOS_CENTERED

static Game::GuiObject* queryObject = nullptr;
static Game::Entity* queryEntity = nullptr;

static void queryObjects();

static std::vector<Key> keySignalQueue;
static std::vector<MouseButton> m_ButtonSignalQueue;
static std::vector<MouseMove> m_MoveSignalQueue;
static std::vector<MouseWheel> m_WheelSignalQueue;

struct _mState
{
    int x, y;
    Uint32 state;
};

static void EngineRun();
static void EngineClose();
static void ProcessInput();
static bool WindowPollEvents();
static SDL_Event sdl_event;


static std::chrono::steady_clock frametime_clock;
static std::chrono::duration<double> targetFrameTime;
const static std::chrono::duration<double> zero = std::chrono::duration<float>::zero();

static char keyboardState[SDL_NUM_SCANCODES];
static _mState mState;

SDL_Window* Engine::Window::sdl_Window;
SDL_Renderer* Engine::Window::sdl_Renderer;

Signal Engine::Input::keyEvent;
Signal Engine::Input::mouseButton;
Signal Engine::Input::mouseWheel;
Signal Engine::Input::mouseMove;

void Engine::Init()
{
    if (SDL_WasInit(0)) return;

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    GameSettings::onSet();

    Game::setWorld<EntryWorld>();
    Game::setGuiLayer<EntryGuiLayer>();

    Window::sdl_Window = SDL_CreateWindow(GameSettings::Title, WCENTERED, WCENTERED, GameSettings::StartResolution.width, GameSettings::StartResolution.height, SDL_WINDOW_RESIZABLE);
    Window::sdl_Renderer = SDL_CreateRenderer(Window::sdl_Window, -1, 0);
    SDL_SetRenderDrawBlendMode(Window::sdl_Renderer, SDL_BLENDMODE_BLEND);

    EngineRun();
    EngineClose();
}

void Engine::setTargetFPS(unsigned short fps)
{
    targetFrameTime = std::chrono::duration<double>(GameSettings::targetFPS > 0 ? (1.0f / GameSettings::targetFPS) : 0);
}

void queryObjects()
{
    queryEntity = nullptr;
    queryObject = nullptr;

    queryObject = Game::currentGuiLayer->_query();
    if (queryObject) return;
    queryEntity = Game::currentWorld->_query();
}

void EngineRun()
{
    static bool wasRun = 0;
    if (wasRun) return;

    wasRun = 1;

    auto start = frametime_clock.now();
    std::chrono::duration<double> delta;

    Engine::setTargetFPS(GameSettings::targetFPS);

    while (WindowPollEvents())
    {
        delta = frametime_clock.now() - start;
        start = frametime_clock.now();
        float dt = delta.count();

        ProcessInput();

        queryObjects();

        Game::currentWorld->Update(dt);
        Game::currentWorld->pUpdate(dt);

        Engine::Window::processGui();
        Engine::Window::render();

        Game::currentGuiLayer->_callUpdate(dt);

        if (targetFrameTime > zero)
        {
            std::chrono::duration<double> frametime = std::chrono::steady_clock::now() - start;

            auto sleeptime = targetFrameTime - frametime;
            std::this_thread::sleep_for(sleeptime);
        }
    }

}

void EngineClose()
{
    if (Game::currentWorld)
        delete Game::currentWorld;
    if (Game::currentGuiLayer)
        delete Game::currentGuiLayer;
    TTF_Quit();
    SDL_Quit();
}

#define processEventQueue(queue, ev) while (!queue.empty()) {\
ev.Fire(&queue.back());\
queue.pop_back();\
}

void ProcessInput()
{
    processEventQueue(keySignalQueue, Engine::Input::keyEvent)
    processEventQueue(m_ButtonSignalQueue, Engine::Input::mouseButton)
    processEventQueue(m_MoveSignalQueue, Engine::Input::mouseMove)
    processEventQueue(m_WheelSignalQueue, Engine::Input::mouseWheel)
}

bool WindowPollEvents()
{
    while (SDL_PollEvent(&sdl_event) != 0)
    {
        switch (sdl_event.type)
        {
        case SDL_QUIT:
            return 0;
        case SDL_MOUSEMOTION:
            m_MoveSignalQueue.emplace_back(&sdl_event.motion);
            break;
        case SDL_MOUSEBUTTONDOWN:
            m_ButtonSignalQueue.emplace_back(&sdl_event.button);
            break;
        case SDL_MOUSEBUTTONUP:
            m_ButtonSignalQueue.emplace_back(&sdl_event.button);
            break;
        case SDL_MOUSEWHEEL:
            m_WheelSignalQueue.emplace_back(&sdl_event.wheel);
            break;
        case SDL_KEYDOWN:
            if (sdl_event.key.keysym.sym > SDL_NUM_SCANCODES)
                keyboardState[sdl_event.key.keysym.scancode] |= 3;
            else
            {
                keyboardState[sdl_event.key.keysym.scancode] |= 1;
                keyboardState[sdl_event.key.keysym.sym] |= 2;
            }
            
            keySignalQueue.emplace_back(&sdl_event.key);
            break;
        case SDL_KEYUP:
            if (sdl_event.key.keysym.sym > 512)
                keyboardState[sdl_event.key.keysym.scancode] &= ~3;
            else
            {
                keyboardState[sdl_event.key.keysym.scancode] &= ~1;
                keyboardState[sdl_event.key.keysym.sym] &= ~2;
            }
            keySignalQueue.emplace_back(&sdl_event.key);
            break;
        default:
            break;
        }
    }

    mState.state = SDL_GetMouseState(&mState.x, &mState.y);

    return 1;
}

bool Engine::Input::isKeyPressed(SDL_KeyCode keycode)
{
    return keyboardState[keycode & ~(1 << 30)] & 2;
}

bool Engine::Input::isKeyPressed(SDL_Scancode scancode)
{
    return keyboardState[scancode] & 1;
}

bool Engine::Input::isMouseButtonPressed(ENUM_MouseButton button)
{
    return mState.state & SDL_BUTTON(button);
}

Vector2i Engine::Input::getMouseScreenPosition()
{
    return Vector2i(mState.x, mState.y);
}

Vector2f Engine::Input::getMouseWorldPosition()
{
    return Game::currentWorld->screenToWorldSpace(mState.x, mState.y);
}

// Check whatever instance the mouse is actually hovering in

Game::Entity* Game::World::_query()
{
    Entity* target_returnEntity = nullptr;

    for (int i = m_Entities.size() - 1; i > 0 && !target_returnEntity; i--)
    {
        for (auto _component : m_Entities[i]->m_Components)
        {
            if (!_component->getType() == SHAPE) continue;
            auto component = (Components::Shape*)_component;
            if (component->isInScreenPoint({ mState.x, mState.y }))
            {
                target_returnEntity = m_Entities[i];
                break;
            }
        }
    }

    return target_returnEntity;
}

Game::GuiObject* Game::GuiContainer::_query()
{
    GuiObject* target_return = nullptr;
    for (int i = p_children.size() - 1; i >= 0 && !target_return; i--)
    {
        if (!p_children[i]->canQuery) continue;
        target_return = p_children[i]->_query();
    }

    if (!target_return && !isGuiLayer())
    {
        SDL_FPoint mousePoint = { mState.x, mState.y };
        if (SDL_PointInFRect(&mousePoint, &p_absolute)) target_return = (GuiObject*)this;
    }

    return target_return;
}

bool Game::GuiObject::isHovering() const
{
    return queryObject == this;
}