#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <src/core/calculation_engine.h>
#include <src/core/cell.h>
#include <src/core/workbook.h>
#include <src/core/worksheet.h>
#include <memory>

namespace excel {
namespace test {

// Mock class for Workbook
class MockWorkbook : public Workbook {
public:
    MOCK_METHOD(Cell*, GetCell, (), (override));
    MOCK_METHOD(Worksheet*, GetWorksheet, (), (override));
};

// Helper function to create a test workbook with predefined data
std::unique_ptr<Workbook> CreateTestWorkbook() {
    // Implementation details omitted for brevity
    return std::make_unique<Workbook>();
}

// Helper function to evaluate a formula in a given cell
CellValue EvaluateFormula(CalculationEngine& engine, Cell& cell) {
    // Implementation details omitted for brevity
    return engine.Evaluate(cell);
}

// Test fixture for CalculationEngine tests
class CalculationEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a mock Workbook
        mock_workbook_ = std::make_unique<MockWorkbook>();
        // Create a CalculationEngine instance
        calculation_engine_ = std::make_unique<CalculationEngine>(*mock_workbook_);
    }

    void TearDown() override {
        // Clean up any resources
        mock_workbook_.reset();
        calculation_engine_.reset();
    }

    std::unique_ptr<MockWorkbook> mock_workbook_;
    std::unique_ptr<CalculationEngine> calculation_engine_;
};

// Test case: Simple addition formula
TEST_F(CalculationEngineTest, SimpleAddition) {
    // Create cells with values 2 and 3
    Cell cell1(1, 1, "2");
    Cell cell2(1, 2, "3");
    EXPECT_CALL(*mock_workbook_, GetCell())
        .WillOnce(::testing::Return(&cell1))
        .WillOnce(::testing::Return(&cell2));

    // Create a cell with formula '=A1+A2'
    Cell formula_cell(1, 3, "=A1+A2");

    // Evaluate the formula
    CellValue result = EvaluateFormula(*calculation_engine_, formula_cell);

    // Assert that the result is 5
    EXPECT_EQ(result.GetNumeric(), 5);
}

// Test case: Complex formula
TEST_F(CalculationEngineTest, ComplexFormula) {
    // Create cells with various values
    // Implementation details omitted for brevity

    // Create a cell with formula '=SUM(A1:A5) * AVERAGE(B1:B3)'
    Cell formula_cell(2, 1, "=SUM(A1:A5) * AVERAGE(B1:B3)");

    // Evaluate the formula
    CellValue result = EvaluateFormula(*calculation_engine_, formula_cell);

    // Assert that the result is correct
    // Assuming the expected result is 100 for this example
    EXPECT_NEAR(result.GetNumeric(), 100.0, 0.001);
}

// Test case: Circular reference
TEST_F(CalculationEngineTest, CircularReference) {
    // Create cells A1 with formula '=B1'
    Cell cell_a1(1, 1, "=B1");
    // Create cells B1 with formula '=A1'
    Cell cell_b1(2, 1, "=A1");

    EXPECT_CALL(*mock_workbook_, GetCell())
        .WillOnce(::testing::Return(&cell_a1))
        .WillOnce(::testing::Return(&cell_b1));

    // Attempt to evaluate A1
    EXPECT_THROW({
        EvaluateFormula(*calculation_engine_, cell_a1);
    }, CircularReferenceException);
}

// Test case: Division by zero
TEST_F(CalculationEngineTest, DivisionByZero) {
    // Create a cell with formula '=1/0'
    Cell formula_cell(1, 1, "=1/0");

    // Evaluate the formula
    CellValue result = EvaluateFormula(*calculation_engine_, formula_cell);

    // Assert that a division by zero error is returned
    EXPECT_TRUE(result.IsError());
    EXPECT_EQ(result.GetError(), CellErrorType::DivisionByZero);
}

} // namespace test
} // namespace excel