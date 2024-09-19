#!/bin/bash

# Set up environment variables
PROJECT_ROOT=$(dirname "$(dirname "$(readlink -f "$0")")")
BUILD_DIR="$PROJECT_ROOT/build"
DEPLOY_CONFIG_DIR="$PROJECT_ROOT/deploy_configs"

# Function to validate the deployment environment
validate_environment() {
    local environment=$1
    if [[ ! "$environment" =~ ^(dev|staging|production)$ ]]; then
        echo "Error: Invalid environment specified. Use dev, staging, or production."
        exit 1
    fi
}

# Function to load the deployment configuration
load_config() {
    local environment=$1
    local config_file="$DEPLOY_CONFIG_DIR/$environment.conf"
    if [ ! -f "$config_file" ]; then
        echo "Error: Configuration file not found for $environment environment."
        exit 1
    fi
    source "$config_file"
}

# Function to create a backup of the current deployment
create_backup() {
    echo "Creating backup of current deployment..."
    ssh "$DEPLOY_SERVER" "cd $DEPLOY_PATH && tar -czf ../backup_$(date +%Y%m%d_%H%M%S).tar.gz ."
    if [ $? -ne 0 ]; then
        echo "Error: Backup creation failed."
        exit 1
    fi
}

# Function to deploy backend components
deploy_backend() {
    echo "Deploying backend components..."
    rsync -avz --delete "$BUILD_DIR/backend/" "$DEPLOY_SERVER:$DEPLOY_PATH/backend/"
    ssh "$DEPLOY_SERVER" "cd $DEPLOY_PATH/backend && ./run_migrations.sh"
    ssh "$DEPLOY_SERVER" "sudo systemctl restart excel-backend.service"
}

# Function to deploy frontend components
deploy_frontend() {
    echo "Deploying frontend components..."
    rsync -avz --delete "$BUILD_DIR/frontend/" "$DEPLOY_SERVER:$DEPLOY_PATH/frontend/"
    ssh "$DEPLOY_SERVER" "cd $DEPLOY_PATH/frontend && ./update_config.sh"
}

# Function to update symlinks
update_symlinks() {
    echo "Updating symlinks..."
    local timestamp=$(date +%Y%m%d_%H%M%S)
    ssh "$DEPLOY_SERVER" "cd $DEPLOY_PATH && mkdir ../$timestamp && mv * ../$timestamp/ && ln -sfn ../$timestamp current"
}

# Function to run smoke tests
run_smoke_tests() {
    echo "Running smoke tests..."
    ./smoke_tests.sh "$DEPLOY_SERVER" "$DEPLOY_PATH"
    if [ $? -ne 0 ]; then
        echo "Error: Smoke tests failed."
        exit 1
    fi
}

# Main script
VERSION=""
FORCE=false
BACKUP=false

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--version)
            VERSION="$2"
            shift 2
            ;;
        -f|--force)
            FORCE=true
            shift
            ;;
        -b|--backup)
            BACKUP=true
            shift
            ;;
        *)
            ENVIRONMENT="$1"
            shift
            ;;
    esac
done

# Validate environment
validate_environment "$ENVIRONMENT"

# Load configuration
load_config "$ENVIRONMENT"

# Create backup if requested
if [ "$BACKUP" = true ]; then
    create_backup
fi

# Confirm deployment if not forced
if [ "$FORCE" = false ]; then
    read -p "Are you sure you want to deploy to $ENVIRONMENT? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Deployment cancelled."
        exit 1
    fi
fi

# Start logging
LOG_FILE="$PROJECT_ROOT/logs/deploy_$(date +%Y%m%d_%H%M%S).log"
exec > >(tee -a "$LOG_FILE") 2>&1

echo "Starting deployment to $ENVIRONMENT environment..."

# Deploy backend
deploy_backend

# Deploy frontend
deploy_frontend

# Update symlinks
update_symlinks

# Run smoke tests
run_smoke_tests

echo "Deployment completed successfully!"

# Log deployed version
if [ -n "$VERSION" ]; then
    echo "Deployed version: $VERSION" >> "$LOG_FILE"
fi

exit 0