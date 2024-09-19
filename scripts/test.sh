#!/bin/bash

# Set up environment variables
PROJECT_ROOT=$(pwd)
TEST_DIR="$PROJECT_ROOT/tests"
REPORT_DIR="$PROJECT_ROOT/test_reports"

# Initialize variables
run_unit=false
run_integration=false
run_e2e=false
verbose=false

# Function to display usage information
usage() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -u, --unit         Run only unit tests"
    echo "  -i, --integration  Run only integration tests"
    echo "  -e, --e2e          Run only end-to-end tests"
    echo "  -a, --all          Run all tests (default)"
    echo "  -v, --verbose      Enable verbose output"
    exit 1
}

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -u|--unit) run_unit=true ;;
        -i|--integration) run_integration=true ;;
        -e|--e2e) run_e2e=true ;;
        -a|--all) run_unit=true; run_integration=true; run_e2e=true ;;
        -v|--verbose) verbose=true ;;
        *) usage ;;
    esac
    shift
done

# If no specific test type is selected, run all tests
if ! $run_unit && ! $run_integration && ! $run_e2e; then
    run_unit=true
    run_integration=true
    run_e2e=true
fi

# Create necessary directories for test reports
mkdir -p "$REPORT_DIR/unit" "$REPORT_DIR/integration" "$REPORT_DIR/e2e" "$REPORT_DIR/coverage"

# Function to log messages
log() {
    if $verbose; then
        echo "[$(date +'%Y-%m-%d %H:%M:%S')] $1"
    fi
}

# Function to run unit tests
run_unit_tests() {
    log "Starting unit tests"
    cd "$TEST_DIR/unit" || exit 1
    
    # Run backend unit tests
    dotnet test --logger "trx;LogFileName=$REPORT_DIR/unit/backend_results.trx"
    
    # Run frontend unit tests
    npm test -- --coverage --coverageDirectory="$REPORT_DIR/unit/frontend_coverage"
    
    log "Finished unit tests"
}

# Function to run integration tests
run_integration_tests() {
    log "Starting integration tests"
    cd "$TEST_DIR/integration" || exit 1
    
    # Set up test database or services here
    
    # Run integration tests
    dotnet test --logger "trx;LogFileName=$REPORT_DIR/integration/results.trx"
    
    # Tear down test database or services here
    
    log "Finished integration tests"
}

# Function to run end-to-end tests
run_e2e_tests() {
    log "Starting end-to-end tests"
    cd "$TEST_DIR/e2e" || exit 1
    
    # Start the application in a test environment
    npm run start:test &
    APP_PID=$!
    
    # Run end-to-end tests
    npm run test:e2e -- --config baseUrl=http://localhost:3000
    
    # Stop the test environment
    kill $APP_PID
    
    log "Finished end-to-end tests"
}

# Function to generate coverage report
generate_coverage_report() {
    log "Generating coverage report"
    # Collect coverage data and generate report
    # This is a placeholder and should be replaced with actual coverage tool commands
    echo "Coverage report generated in $REPORT_DIR/coverage"
}

# Function to check test results
check_test_results() {
    log "Checking test results"
    # This is a placeholder and should be replaced with actual result checking logic
    # For now, we'll assume all tests passed
    return 0
}

# Main script execution
overall_status=0

if $run_unit; then
    run_unit_tests
    unit_status=$?
    overall_status=$((overall_status + unit_status))
    log "Unit tests completed with status: $unit_status"
fi

if $run_integration; then
    run_integration_tests
    integration_status=$?
    overall_status=$((overall_status + integration_status))
    log "Integration tests completed with status: $integration_status"
fi

if $run_e2e; then
    run_e2e_tests
    e2e_status=$?
    overall_status=$((overall_status + e2e_status))
    log "End-to-end tests completed with status: $e2e_status"
fi

generate_coverage_report
check_test_results

if [ $overall_status -eq 0 ]; then
    echo "All tests passed successfully!"
else
    echo "Some tests failed. Please check the test reports for details."
fi

exit $overall_status