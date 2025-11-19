# API Documentation

This directory contains **Doxygen-generated API documentation** for the PID controller.

## Generating Documentation

To generate the API documentation:

```bash
# From repository root
doxygen Doxyfile
```

This will create:
- `docs/api/html/index.html` - HTML documentation (open in browser)
- `docs/api/latex/` - LaTeX documentation (optional)

## Viewing Documentation

### Local Viewing
```bash
# Open in default browser (macOS)
open docs/api/html/index.html

# Open in default browser (Linux)
xdg-open docs/api/html/index.html

# Open in default browser (Windows)
start docs/api/html/index.html
```

### Online Viewing

API documentation can be hosted on GitHub Pages alongside the main documentation at:
https://onesmoogore.github.io/embedded-motor-pid-controller/api/html/

## What's Included

The API documentation includes:
- **Function Reference** - All public API functions
- **Data Structures** - PID controller structures and types
- **File Documentation** - Source file descriptions
- **Call Graphs** - Function dependency visualization
- **Usage Examples** - Code snippets and examples

## Requirements

To generate documentation, you need:
- Doxygen (latest version recommended)
- Graphviz (for diagrams and call graphs)

### Installation

**macOS:**
```bash
brew install doxygen graphviz
```

**Ubuntu/Debian:**
```bash
sudo apt-get install doxygen graphviz
```

**Windows:**
Download from:
- Doxygen: https://www.doxygen.nl/download.html
- Graphviz: https://graphviz.org/download/

## Note

This directory is excluded from version control (see `.gitignore`).
Generated documentation is created locally and can be deployed to GitHub Pages.
