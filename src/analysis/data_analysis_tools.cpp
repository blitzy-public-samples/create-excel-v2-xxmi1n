#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include "excel_types.h"
#include "calculation_engine.h"
#include "data_analysis_tools.h"

const double EPSILON = 1e-10;

DataAnalysisTools::DataAnalysisTools(std::shared_ptr<CalculationEngine> calculationEngine)
    : m_calculationEngine(calculationEngine) {}

RegressionResult DataAnalysisTools::PerformRegression(const CellRange& xRange, const CellRange& yRange) {
    // Validate input ranges
    if (!ValidateRanges(xRange, yRange)) {
        throw std::invalid_argument("Invalid input ranges for regression analysis");
    }

    // Extract x and y values from the ranges
    std::vector<double> xValues, yValues;
    for (const auto& cell : xRange) {
        xValues.push_back(cell.GetNumericValue());
    }
    for (const auto& cell : yRange) {
        yValues.push_back(cell.GetNumericValue());
    }

    // Calculate regression coefficients
    auto [slope, intercept] = CalculateRegressionCoefficients(xValues, yValues);

    // Calculate regression statistics
    auto stats = CalculateRegressionStatistics(xValues, yValues, slope, intercept);

    // Construct and return RegressionResult object
    return RegressionResult{slope, intercept, stats};
}

ANOVAResult DataAnalysisTools::PerformANOVA(const std::vector<CellRange>& dataSets) {
    // Validate input data sets
    if (!ValidateANOVAInput(dataSets)) {
        throw std::invalid_argument("Invalid input data sets for ANOVA");
    }

    // Extract values from the data sets
    std::vector<std::vector<double>> values;
    for (const auto& dataSet : dataSets) {
        std::vector<double> dataSetValues;
        for (const auto& cell : dataSet) {
            dataSetValues.push_back(cell.GetNumericValue());
        }
        values.push_back(dataSetValues);
    }

    // Calculate ANOVA statistics
    auto stats = CalculateANOVAStatistics(values);

    // Construct and return ANOVAResult object
    return ANOVAResult{stats};
}

DescriptiveStatistics DataAnalysisTools::CalculateDescriptiveStatistics(const CellRange& dataRange) {
    // Validate input range
    if (!ValidateRange(dataRange)) {
        throw std::invalid_argument("Invalid input range for descriptive statistics");
    }

    // Extract values from the data range
    std::vector<double> values;
    for (const auto& cell : dataRange) {
        values.push_back(cell.GetNumericValue());
    }

    // Calculate statistics
    double mean = CalculateMean(values);
    double median = CalculateMedian(values);
    double stdDev = CalculateStandardDeviation(values, mean);
    double min = *std::min_element(values.begin(), values.end());
    double max = *std::max_element(values.begin(), values.end());

    // Construct and return DescriptiveStatistics object
    return DescriptiveStatistics{mean, median, stdDev, min, max};
}

bool ValidateRanges(const CellRange& xRange, const CellRange& yRange) {
    // Check if ranges are empty
    if (xRange.empty() || yRange.empty()) {
        return false;
    }

    // Verify that ranges have the same number of cells
    if (xRange.size() != yRange.size()) {
        return false;
    }

    // Ensure all cells contain numeric values
    for (const auto& cell : xRange) {
        if (!cell.IsNumeric()) {
            return false;
        }
    }
    for (const auto& cell : yRange) {
        if (!cell.IsNumeric()) {
            return false;
        }
    }

    return true;
}

std::pair<double, double> CalculateRegressionCoefficients(const std::vector<double>& xValues, const std::vector<double>& yValues) {
    double xMean = CalculateMean(xValues);
    double yMean = CalculateMean(yValues);

    double sumXY = 0.0, sumX2 = 0.0;
    for (size_t i = 0; i < xValues.size(); ++i) {
        sumXY += (xValues[i] - xMean) * (yValues[i] - yMean);
        sumX2 += std::pow(xValues[i] - xMean, 2);
    }

    double slope = sumXY / sumX2;
    double intercept = yMean - slope * xMean;

    return {slope, intercept};
}

RegressionStatistics CalculateRegressionStatistics(const std::vector<double>& xValues, const std::vector<double>& yValues, double slope, double intercept) {
    double sse = 0.0, sst = 0.0;
    double yMean = CalculateMean(yValues);

    for (size_t i = 0; i < xValues.size(); ++i) {
        double yPred = slope * xValues[i] + intercept;
        sse += std::pow(yValues[i] - yPred, 2);
        sst += std::pow(yValues[i] - yMean, 2);
    }

    double rSquared = 1.0 - (sse / sst);
    double standardError = std::sqrt(sse / (xValues.size() - 2));

    // Calculate t-statistics and F-statistic
    // Note: These calculations are simplified and may need to be expanded for a complete implementation

    return RegressionStatistics{rSquared, standardError, 0.0, 0.0, 0.0};
}

bool ValidateANOVAInput(const std::vector<CellRange>& dataSets) {
    // Check if there are at least two data sets
    if (dataSets.size() < 2) {
        return false;
    }

    // Verify that all data sets contain numeric values and have at least one value
    for (const auto& dataSet : dataSets) {
        if (dataSet.empty()) {
            return false;
        }
        for (const auto& cell : dataSet) {
            if (!cell.IsNumeric()) {
                return false;
            }
        }
    }

    return true;
}

ANOVAStatistics CalculateANOVAStatistics(const std::vector<std::vector<double>>& dataSets) {
    // Calculate overall mean
    double overallMean = 0.0;
    int totalCount = 0;
    for (const auto& dataSet : dataSets) {
        overallMean += std::accumulate(dataSet.begin(), dataSet.end(), 0.0);
        totalCount += dataSet.size();
    }
    overallMean /= totalCount;

    // Calculate sum of squares between groups (SSB) and within groups (SSW)
    double ssb = 0.0, ssw = 0.0;
    for (const auto& dataSet : dataSets) {
        double groupMean = CalculateMean(dataSet);
        ssb += dataSet.size() * std::pow(groupMean - overallMean, 2);
        for (const auto& value : dataSet) {
            ssw += std::pow(value - groupMean, 2);
        }
    }

    // Calculate degrees of freedom
    int dfb = dataSets.size() - 1;
    int dfw = totalCount - dataSets.size();

    // Calculate mean squares
    double msb = ssb / dfb;
    double msw = ssw / dfw;

    // Calculate F-statistic
    double fStatistic = msb / msw;

    // Calculate p-value (simplified, actual implementation would use F-distribution)
    double pValue = 0.0;  // Placeholder

    return ANOVAStatistics{ssb, ssw, dfb, dfw, msb, msw, fStatistic, pValue};
}

double CalculateMean(const std::vector<double>& values) {
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

double CalculateMedian(std::vector<double> values) {
    std::sort(values.begin(), values.end());
    size_t size = values.size();
    if (size % 2 == 0) {
        return (values[size / 2 - 1] + values[size / 2]) / 2.0;
    } else {
        return values[size / 2];
    }
}

double CalculateStandardDeviation(const std::vector<double>& values, double mean) {
    double sumSquaredDiff = 0.0;
    for (const auto& value : values) {
        sumSquaredDiff += std::pow(value - mean, 2);
    }
    return std::sqrt(sumSquaredDiff / (values.size() - 1));
}