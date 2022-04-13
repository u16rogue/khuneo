# khuneo
[GitLab](https://gitlab.com/u16rogue/khuneo) • [GitHub](https://github.com/u16rogue/khuneo)<br>
A modular and abstracted embedded programming language for C++

## About
khuneo is an embeddable programming language for C++ (cxx20+). Each implementation is abstracted
allowing developers to shape khuneo to their needs and constraints, by design every* aspect and behavior of khuneo can be extended and modified.

<!--

* khuneo is bloat free in a sense that everything in the language is provided as is.
* Flexible, Extensible, Modular, and Easy to use and embed.
* JIT Compiled (Automated, Requested)
* Low level access (Generate native functions, inline assembly, direct pointer (+structure) read and write)
* Intercepts, Interop +/ Reflection

## Snippet
```
import stdlib as std
{
    version: >= 2,
    required
};

@entrypoint
fn main() i32
{
    std.print("Hello world!");
    return 0;
}

```

## Embedding

CMakeLists.txt
```cmake
...
set(CMAKE_CXX_STANDARD 20)
add_subdirectory("dependencies/khuneo")
...
target_link_libraries(${PROJECT_NAME} PRIVATE khuneo)
...
```

main.cpp
```cpp
#include <khuneo/khuneo.hpp>
#include <cstdio.h>

// This is entirely optional, you can instead just use khuneo::container<> immediately
class my_custom_implementation : public khuneo::container<my_custom_extension>
{
    // Overrides the container that khuneo uses to store states
    using kh_state_container_t = std::list<khuneo::container<>::state>;

    // Overrides the allocation method khuneo uses
    static auto kh_allocate(khuneo::internal::allocreq * r) -> bool
    {
        printf("Khuneo wants to allocate %d bytes of memory!", r->size);

        // You can then implement your own allocation method
        // r->loc = new char[r->size];
        // if (!r->loc)
        //    return false;
        // return true;

        // Or if you just wanted to see if it occurs and want to call
        // the original implementation
        return khuneo::container<>::kh_allocate(r);
    }

    static auto kh_error(khuneo::error e) -> bool
    {
        printf("khuneo encountered an error: %s", khuneo::extra::format(e));
        return false; // Do not continue
    }
}

auto main() -> int
{
    my_custom_implementation c;

    khuneo::module s1 = c.create_module();
    s1.load_file("stdlib.kun");

    khuneo::module s2 = c.create_module();
    s2.load_file("helloworld.kun");

    khuneo::thread_context tc(s2);
    tc.invoke<khuneo::ANN_ENTRYPOINT>();

    return 0;
}
```
-->

## Dependencies
* None

## License
* GNU Lesser General Public License v2.1