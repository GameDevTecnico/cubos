# Contribution Guidelines

## Pull Requests

- Pull requests should have a clear description of what they add.
- When working on new features, draft PRs should be created so that other contributors can have an idea of what is being worked on.
- Draft PRs' titles should start with `[WIP]` to indicate that they are WIP.

## Documentation

- Everything should be well documented (`///` style, check already existing files).
- Be brief and consistent.
- Keep the documentation up to date when making changes.

## Code Style

- Code must be formatted using [clang format](https://clang.llvm.org/docs/ClangFormat.html).
- `camelCase` is used for functions, methods, local variables and fields (prefixed with 'm' on private fields).
- `PascalCase` is used for class names and constants.
- `UPPER_CASE` is used for macros.
- `snake_case` is used for namespaces, folders and files.
- Each file should have an `#ifndef` include guard.
- When closing a namespace the namespace name should be added as a comment, the same goes for if macros (`#ifdef`, ...)
- Namespaces should match the directory structure, except `namespace impl`, which can be used to hide implementation details (check `cubos::gl::impl`).
