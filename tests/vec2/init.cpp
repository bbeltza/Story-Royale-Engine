#ifdef NDEBUG
    #undef NDEBUG // Tests should be interpreted as in debug mode, even on release
#endif
#include <Entry.h>
#include <datatypes/vector.hpp>

#define TEST_FAIL "Test failed (static assertion)"

void sre::initialize()
{
    using namespace sre;
    // Every single test is independent and thus will have its own scope

    // Constructors
        // Default
        {
            vec2i vec2test;
            static_assert(std::is_same<vec2i::type, int>::value, TEST_FAIL);
            assert(vec2test.x == 0 && vec2test.y == 0);
        }
        // X and Y

        // Scalar
        // Copy

        // Conversion
    
    // Operations
        // Function operations
        // Type agnostic function operations

        // Function operation getters
        // + Type checking

        // Overloaded operators
        // + Type checking
    
    // Math operations (e.g. abs & dot)
    
    // Equality overloaded operators ( == and != )

    // Constant expression checks
    {
        static constexpr vec2i constexprtest{100, 100};
        static_assert(constexprtest.x == 100 && constexprtest.y == 100, TEST_FAIL);
    }
    exit(0);
}