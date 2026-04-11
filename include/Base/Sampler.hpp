#ifndef SRE_SAMPLER_HPP
#define SRE_SAMPLER_HPP
#include <Core/Render.h>

namespace sre
{
    // RAII sampler class, designed to replace sre::Texture
    class RSampler
    {
        sre::Sampler* m_ptr = NULL;

        public:
            constexpr RSampler() = default;
            RSampler(sre::Sampler* sampler): m_ptr(sampler) {}
            ~RSampler() { sre::get_renderer()->sampler(m_ptr); }

            RSampler(const RSampler&) = delete;
            RSampler(RSampler&& right): m_ptr(right.m_ptr) { right.m_ptr = NULL; }

            void operator =(RSampler&& right)
            {
                if (m_ptr)
                    sre::get_renderer()->sampler(m_ptr);
                m_ptr = right.m_ptr;
                right.m_ptr = NULL;
            }

            operator sre::Sampler*() const { return m_ptr; }
            operator bool() const { return m_ptr != NULL; }
    };
}

#endif