#!/bin/bash

# Set up environment variables
PROJECT_ROOT=$(dirname "$(dirname "$0")")
BUILD_DIR="$PROJECT_ROOT/build"
SOURCE_DIR="$PROJECT_ROOT/src"

# Function to clean the build directory
clean_build() {
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"/*
    mkdir -p "$BUILD_DIR"/{backend,frontend,desktop}
}

# Function to build backend components
build_backend() {
    echo "Building backend..."
    cd "$SOURCE_DIR/backend" || exit 1
    dotnet build "${BUILD_FLAGS[@]}"
    cp -r bin/* "$BUILD_DIR/backend/"
}

# Function to build frontend components
build_frontend() {
    echo "Building frontend..."
    cd "$SOURCE_DIR/web" || exit 1
    npm install
    npm run build -- "${BUILD_FLAGS[@]}"
    cp -r dist/* "$BUILD_DIR/frontend/"
}

# Function to build desktop application
build_desktop() {
    echo "Building desktop application..."
    cd "$SOURCE_DIR/desktop" || exit 1
    # Platform-specific build commands (example for Windows using MSBuild)
    if [[ "$OSTYPE" == "msys"* ]]; then
        MSBuild.exe ExcelApp.sln "${BUILD_FLAGS[@]}"
        cp -r bin/* "$BUILD_DIR/desktop/"
    else
        echo "Desktop build not implemented for this platform"
    fi
}

# Function to run tests
run_tests() {
    echo "Running tests..."
    cd "$PROJECT_ROOT/tests" || exit 1
    dotnet test
    npm test
}

# Parse command-line arguments
BUILD_MODE="release"
CLEAN_BUILD=false
BUILD_FLAGS=()

while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--debug)
            BUILD_MODE="debug"
            BUILD_FLAGS+=("--configuration" "Debug")
            shift
            ;;
        -r|--release)
            BUILD_MODE="release"
            BUILD_FLAGS+=("--configuration" "Release")
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Main build script
echo "Starting build process in $BUILD_MODE mode..."

if $CLEAN_BUILD; then
    clean_build
fi

build_backend
if [ $? -ne 0 ]; then
    echo "Error: Backend build failed"
    exit 1
fi

build_frontend
if [ $? -ne 0 ]; then
    echo "Error: Frontend build failed"
    exit 1
fi

build_desktop
if [ $? -ne 0 ]; then
    echo "Error: Desktop build failed"
    exit 1
fi

if [ "$BUILD_MODE" != "debug" ]; then
    run_tests
    if [ $? -ne 0 ]; then
        echo "Error: Tests failed"
        exit 1
    fi
fi

echo "Build completed successfully!"
echo "Build artifacts can be found in $BUILD_DIR"