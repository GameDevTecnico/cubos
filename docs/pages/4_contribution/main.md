# Contribution guidelines {#contribution}

@brief Guide on the project's conventions and code style.

## Commits

Commits should be concise and small, such that they are easy to review.
Avoid at all costs including non-related changes in a commit - use `git add -p`
or something similar to force yourself to review the changes you are
commiting, and to avoid accidentally commiting unrelated changes.

Commits should follow [Conventional Commits](https://www.conventionalcommits.org/)
and be written in imperative mood. As scopes, we use the names of the labels
that start by `B-` in the [label list](https://github.com/GameDevTecnico/cubos/labels)
(e.g.: `assets`, `audio`). If the commit affects multiple of those areas, then
pick one of the labels that start by `A-` (e.g.: `core`, `engine`, `tesseratos`).
If a commit affects more than one of those, then you can ommit the scope.

If your description is too long, you can add a body to the commit message.
The body should be separated from the description by a blank line.

Examples of good commit messages:

```txt
feat(core): add CUBOS_FAIL, CUBOS_UNREACHABLE and CUBOS_DEBUG_ASSERT 
test(core): move filesystem tests to data/fs
fix(core): fix segfault when popping a sub context
feat(engine): implement system for sweeping the markers  
docs(engine): add comments to colliders
chore: replace GoogleTest submodule with doctest
```

Examples of bad commit messages:

```txt
fix: fix bug
make it work
feat(core): Add CUBOS_FAIL
docs(engine): added comments to colliders
```

## Pull Requests

Pull requests should be concise and small, and if needed, split into multiple
smaller PRs so that they are easier to review. If your PR is still not
finished, mark it as a draft. When working on new features, draft PRs should
be created so that other contributors can have an idea of what is being worked
on.

Any features added in a PR should be covered by tests and documentation,
including new examples demonstrating the feature.

## Documentation

We use [Doxygen](https://www.doxygen.nl/index.html) for documentation, which
means that the documentation is mostly written in the code itself. When adding
new files, classes, functions, etc, make sure to at least add a triple slash
comment (`///`) with a `@brief` section describing what it does, otherwise
Doxygen will omit it from the documentation.

Make sure to document all function parameters, template parameters and return
values. Take a look at other files to get a grasp of the documentation style
we use.

When changing the code, the documentation should be updated accordingly.

### Plugins

Engine plugins should document which components and resources they add, which
tags and settings are used, and how to use them. Take a look at the
documentation of other plugins such as the @ref renderer-plugin "renderer plugin"
to get an idea of how it should look like.

Every type or function that is part of the public API of a plugin should be
added to its corresponding `group` in the Doxygen documentation, using the
`@ingroup` tag.

## Code Style

### Casing

We use `camelCase` for functions, methods, local variables and fields. Private
fields are prefixed with `m` (e.g. `mMyField`). `PascalCase` is used for class
names and constants. `UPPER_CASE` is used for macros. `snake_case` is used for
namespaces, folders and files.

### Formatting

Code is formatted using [clang format](https://clang.llvm.org/docs/ClangFormat.html).
Although we have an action that runs clang format on every PR, it is
recommended that you run it locally before commiting your changes, to make it
easier for the reviewers to read your code.

We also check the code with [clang tidy](https://clang.llvm.org/extra/clang-tidy/),
which is a static analysis tool which picks up many common mistakes and code
smell. This runs on every commit you push to your branch.

### Macros

Avoid using macros whenever possible - use `constexpr` variables or functions
instead. If you do need to use a macro to make an implementation more readable,
restrict the macro to the source file where it is used. Defining macros in
header files is heavily discouraged.

### Namespaces

Avoid `using namespace` in header files. In source files, prefer
`using foo::bar::X` over `using namespace foo::bar`. If your code is under the
namespace `foo::bar`, you can use `using namespace foo::bar` in the source
files, to make the code more readable.

When closing a namespace the namespace name should be added as a comment. E.g.:

```cpp
namespace foo
{
    ...
} // namespace foo
```
