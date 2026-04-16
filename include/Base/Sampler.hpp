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
            RSampler(sre::Sampler* sampler): m_ptr(sampler) { sampler->aquire(); }
            ~RSampler() { m_ptr->release(); }

            RSampler(const RSampler& copy): m_ptr(copy.m_ptr) { m_ptr->aquire(); };
            RSampler(RSampler&& right): m_ptr(right.m_ptr) { right.m_ptr = NULL; }

            void operator =(RSampler&& right) {
                if (m_ptr)
                    m_ptr->release();
                m_ptr = right.m_ptr;
                right.m_ptr = NULL;
            }

            void operator =(const RSampler& copy) {
                if (m_ptr)
                    m_ptr->release();
                m_ptr = copy.m_ptr;
                m_ptr->aquire();
            }
            
            sre::Sampler* get() const { return m_ptr; }
            sre::Sampler* operator ->() const { return m_ptr; }
            operator bool() const { return m_ptr != NULL; }
    };
}

#endif