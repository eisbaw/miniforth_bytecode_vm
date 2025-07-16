# Claude Development Guidelines

This file contains important guidelines for AI assistants working on the Miniforth Bytecode VM project.

## Core Principles

### 1. Local-First Development
- **Primary workflow**: Use `nix-shell` for reproducible development environment
- **Task runner**: Use `just` for all build/test/lint operations
- **CI approach**: GitHub Actions only calls `nix-shell --run 'just <command>'`
- **Rationale**: Ensures perfect dev/CI parity - anything that works locally works in CI

### 2. Output Verbosity Guidelines
- **NEVER add verbose echo statements** to justfile recipes or shell.nix
- **Avoid chatty output** in build scripts and configuration files
- **Keep output minimal** - only show essential information and errors
- **Use consistent formatting** - emojis sparingly, clear status indicators (✅/❌)

### 3. Development Environment
- **shell.nix**: Minimal, focused on essential environment setup
- **justfile**: Commands should be concise with minimal output
- **CI jobs**: Use existing justfile recipes, don't duplicate logic
- **Tool integration**: All development tools available through Nix

## Project Structure

### Build System
- **CMake**: Main build system for C code
- **Nix**: Reproducible dependency management
- **Just**: Task runner for common operations
- **LCC**: C compiler with custom Miniforth backend

### Code Organization
- `src/`: Main source code (compiler, VM, tools)
- `tests/`: Unit tests and integration tests
- `tests/lcc_miniforth/`: Comprehensive LCC test suite (31 files)
- `external/lcc/`: LCC compiler with Miniforth backend
- `.github/workflows/`: CI configuration

## Development Workflow

### Standard Commands
```bash
# Enter development environment
nix-shell

# Build everything
just build

# Run tests
just test

# Static analysis
just lint

# Format code
just format
```

### CI Integration
- All CI jobs use: `nix-shell --run 'just <command>'`
- No duplicate logic between local and CI
- CI jobs: build, lint, test, lcc-tests, coverage, benchmark, security

## Code Quality Standards

### Static Analysis
- **cppcheck**: Comprehensive static analysis with appropriate suppressions
- **clang-format**: Consistent code formatting
- **const correctness**: Use const parameters and variables where appropriate
- **Error handling**: Proper error checking and reporting

### Testing
- **Unit tests**: VM operations and compiler components
- **Integration tests**: Full compilation pipeline
- **LCC tests**: 31 comprehensive test cases across categories
- **Stress tests**: Memory, recursion, algorithmic complexity

### Documentation
- **README.md**: User-facing documentation with examples
- **Code comments**: Minimal, focused on why not what
- **API documentation**: Clear function signatures and behavior

## Specific Guidelines

### Justfile Rules
- **No command hiding**: Never use `@` prefix - show all commands being executed
- **Error handling**: Use `-` prefix for commands that may fail (like `clean`)
- **Complete documentation**: Every recipe must have a descriptive comment
- **Modular organization**: Use `mod` for logical grouping of related recipes
- **No default recipes**: Remove `default` recipes - users should explicitly choose what to run
- **Consistent formatting**: Group related commands logically with section headers
- **Avoid redundant status messages**: Let commands show their own output

### Shell.nix Rules
- Minimal shellHook (no verbose output)
- Include only essential development tools
- Set necessary environment variables
- Create build directory if needed

### CI Configuration
- Use Ubuntu latest with Nix
- Leverage Cachix for build caching
- Separate jobs for different concerns
- Upload artifacts for important outputs
- Run security scans on main branch

## Compiler Architecture

### Two Compilers
1. **Custom Compiler** (1,931 lines): Educational, minimal dependencies
2. **LCC Backend** (284 lines + 58,891 LCC): Production-ready, full C89

### VM Design
- **Stack-based**: 16-bit instruction set
- **Miniforth**: Forth-inspired bytecode format
- **Multiple variants**: Different size/performance tradeoffs
- **Comprehensive instruction set**: Arithmetic, memory, control flow

## Anti-Patterns to Avoid

### ❌ Don't Do This
- Hide commands with `@` prefix in justfile
- Skip error handling (`-` prefix) for fallible commands
- Leave recipes undocumented
- Add `default` recipes that just list other recipes
- Chatty shell.nix configuration
- Duplicate CI logic outside of justfile
- Inconsistent output formatting
- Ignoring static analysis warnings
- Complex build scripts outside of CMake

### ✅ Do This Instead
- Show all commands being executed (no `@` prefix)
- Use `-` prefix for commands that may fail
- Document every recipe with clear comments
- Use justfile modules for organization
- Let users explicitly choose recipes to run
- Trust the local-first approach
- Consistent error handling
- Address all linter warnings
- Keep build logic in CMake

## Future Considerations

### Potential Enhancements
- JIT compilation for performance
- Additional VM instruction variants
- More comprehensive benchmarking
- Interactive debugger improvements
- Additional language frontends

### Maintenance Guidelines
- Keep dependencies minimal
- Maintain test coverage
- Update documentation with changes
- Monitor CI performance
- Regular security updates

## Testing Strategy

### Test Categories
- **Basic operations**: arithmetic, control_flow, functions, variables
- **Advanced features**: algorithms, stress, data_structures, edge_cases
- **Mathematical**: matrix operations, number theory, fixed-point arithmetic
- **Memory management**: large arrays, deep recursion, call chains

### Test Execution
- Local testing: `just test`
- Category testing: `just test-category <category>`
- LCC testing: `just test-lcc-miniforth`
- CI validation: `just validate-ci`

## Remember

This is a **research project** exploring minimal VM implementations and compiler retargeting. The goal is educational and experimental, not production deployment. Keep the codebase clean, well-tested, and easy to understand.

---

*This file should be updated as the project evolves. Always maintain the local-first development principle and minimal output philosophy.*