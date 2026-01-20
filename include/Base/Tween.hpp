#ifndef SRE_TWEEN_HPP
#define SRE_TWEEN_HPP

#include <Base/Signal.hpp>
#include <utils/math.hpp>
#include <Datatypes/TweenStyle.h>

namespace sre
{
    class TweenServer;

    class Tween
    {
    public:
        const TweenStyle& style;
        sre::Signal<empty_t> completed{this};
    private:
        sre::timeStamp m_elapsed = 0;
        int m_state = 0;
    public:
        Tween& play() { m_state = 1; if (!m_elapsed) start(); return *this; }
        // Pause the current tween animation
        // @return The time the tween has been playing for
        sre::timeStamp pause() { m_state = 0; return m_elapsed; }
        sre::timeStamp cancel() { sre::timeStamp tmp = pause(); m_elapsed = 0; return tmp; }
    protected:
        virtual void step(sre::timeStamp alpha) = 0;
        virtual void start() = 0;
        Tween(const TweenStyle& style): style(style) {}
    private:
        friend class TweenServer;
    };

    // Class container for tweens
    // Store it somewhere, add your tween animations, and call update() every frame to animate those tweens
    class TweenServer
    {
        std::vector<std::shared_ptr<Tween>> m_tweens;
    public:
        void update();
    private:
        template <typename T, typename GoalType>
        class tw: public Tween
        {
            T& m_target;

            GoalType m_goal;
            T m_beginning;

            void step(sre::timeStamp alpha) override { m_target = ut::lerp(m_beginning, m_goal, alpha); }
            void start() override { m_beginning = m_target; }
        public:
            tw(const TweenStyle& style, const GoalType& goal, T& target): Tween(style), m_goal(goal), m_target(target) {}
        };

        template <typename T, typename GoalType>
        std::shared_ptr<Tween> add_tw(const TweenStyle& style, const GoalType& goal, T& target)
        {
            using tw_type = tw<T, GoalType>;

            tw_type* tw = new tw_type(style, goal, target);
            m_tweens.emplace_back(tw);

            return m_tweens.back();
        }
    public:
        template <typename T, typename T2>
        std::shared_ptr<Tween> tween(const TweenStyle& style, T& target, const T2& goal) { return add_tw<T, T>(style, goal, target); }

        template <typename T, typename T2>
        std::shared_ptr<Tween> tween(const TweenStyle& style, T& target, const T2* goal) { return add_tw<T, const T&>(style, *goal, target); }

        void clear() { m_tweens.clear(); }
    };
}

#endif