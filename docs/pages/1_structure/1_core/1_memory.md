# Memory {#core-memory}

The namespace \ref cubos::core::memory provides a stream library
and memory utilities.

## Streams

The main reason for us to provide our custom stream library comes from our
virtual file system and serialization system. 

1. The data read from a virtual file doesn't come, necessarily, from a real
file. It may even be read from a in-memory buffer (for example, when reading an
embedded file).
2. The serializer doesn't care where the output data is going to and the
deserializer doesn't care where the input data is coming from.

With this in mind, it makes sense to have an abstract
\ref cubos::core::memory::Stream "Stream" interface. Examples of stream
implementations are:
- \ref cubos::core::memory::BufferStream "BufferStream" - a stream which
reads/writes data from/to a buffer.
- \ref cubos::core::memory::StandardStream "StandardStream" - a stream which reads/writes
data from/to a standard C stream (basically, a `FILE*` wrapper).
- \ref cubos::core::data::FileStream "FileStream" - a stream which reads/writes
data from/to a virtual file.

Sometimes its useful to have a **CUBOS.** stream connected to either `stdin`,
`stdout` or `stderr`. For this reason,
\ref cubos::core::memory::Stream "Stream" provides
the static members \ref cubos::core::memory::Stream::stdIn "Stream::stdIn",
\ref cubos::core::memory::Stream::stdOut "Stream::stdOut" and
\ref cubos::core::memory::Stream::stdErr "Stream::stdErr".

### Using a stream

The following is an example use case of streams. Let's say we have a function
`helloWorld` which takes a \ref cubos::core::memory::Stream "Stream&" as an
argument, and writes the string `"Hello, world!"` to the stream.

```cpp
#include <cubos/core/memory/stream.hpp>

using namespace cubos::core;

void helloWorld(memory::Stream& stream)
{
    stream.print("Hello, world!");
}
```

If we now wanted to write the string to the standard output, we could just
call:
    
```cpp
helloWorld(memory::Stream::stdOut);
```

If we wanted to write the string to a buffer, we could use a
\ref cubos::core::memory::BufferStream "BufferStream":

```cpp
using namespace cubos::core;

char buf[256];
auto stream = memory::BufferStream(buf, sizeof(buf));
helloWorld(stream);
stream.put('\0'); // Add a null terminator.
// Now we can use buf as a string.
```

One of the common mistakes to be aware of is forgetting to handle different
[endianness](https://en.wikipedia.org/wiki/Endianness) when writing binary
data (e.g.: using \ref cubos::core::memory::Stream::write "Stream::write"
directly). **CUBOS.** provides utility endianness functions in
`cubos/core/memory/endianness.hpp`, such as:
- \ref cubos::core::memory::toBigEndian "toBigEndian" - converts a value from
the host endianness to big endian.
- \ref cubos::core::memory::toLittleEndian "toLittleEndian" - converts a value
from the host endianness to little endian.
- and so on.

For further details, see the methods of
\ref cubos::core::memory::Stream "Stream".
