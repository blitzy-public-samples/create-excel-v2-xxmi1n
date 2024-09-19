// Import required dependencies
const { By, until } = require('selenium-webdriver');
const { Driver } = require('selenium-webdriver/chrome');
const { expect } = require('chai');
const { initializeDriver, waitForElementVisible, waitForElementClickable, getCellValue } = require('../utils/test_helper');

// Define constants
const BASE_URL = 'https://excel.example.com';
const DEFAULT_TIMEOUT = 10000;

// Helper functions
async function createNewWorkbook(driver) {
  // Click on 'New Workbook' button
  const newWorkbookButton = await driver.findElement(By.css('[data-testid="new-workbook-button"]'));
  await newWorkbookButton.click();

  // Wait for the workbook to load
  await driver.wait(until.elementLocated(By.css('[data-testid="workbook-title"]')), DEFAULT_TIMEOUT);

  // Return the workbook title
  const workbookTitle = await driver.findElement(By.css('[data-testid="workbook-title"]')).getText();
  return workbookTitle;
}

async function enterFormula(driver, cellId, formula) {
  // Locate the cell by ID
  const cell = await driver.findElement(By.id(cellId));

  // Click on the cell
  await cell.click();

  // Enter the formula
  await driver.actions().sendKeys(formula).perform();

  // Press Enter to confirm
  await driver.actions().sendKeys('\n').perform();
}

async function addNewSheet(driver) {
  // Click on the 'Add Sheet' button
  const addSheetButton = await driver.findElement(By.css('[data-testid="add-sheet-button"]'));
  await addSheetButton.click();

  // Wait for the new sheet to be created
  await driver.wait(until.elementLocated(By.css('[data-testid="new-sheet-name"]')), DEFAULT_TIMEOUT);

  // Return the new sheet name
  const newSheetName = await driver.findElement(By.css('[data-testid="new-sheet-name"]')).getText();
  return newSheetName;
}

describe('Formula Calculation Tests', function() {
  let driver;

  before(async function() {
    // Initialize the WebDriver
    driver = await initializeDriver();

    // Navigate to the Excel web application
    await driver.get(BASE_URL);

    // Create a new workbook
    await createNewWorkbook(driver);
  });

  after(async function() {
    // Close the browser
    if (driver) {
      await driver.quit();
    }
  });

  it('Basic arithmetic formula calculation', async function() {
    // Enter value '10' in cell A1
    await enterFormula(driver, 'A1', '10');

    // Enter value '20' in cell A2
    await enterFormula(driver, 'A2', '20');

    // Enter formula '=A1+A2' in cell A3
    await enterFormula(driver, 'A3', '=A1+A2');

    // Verify cell A3 displays the result '30'
    const result = await getCellValue(driver, 'A3');
    expect(result).to.equal('30');
  });

  it('Complex formula with multiple operations', async function() {
    // Enter value '5' in cell B1
    await enterFormula(driver, 'B1', '5');

    // Enter value '3' in cell B2
    await enterFormula(driver, 'B2', '3');

    // Enter value '2' in cell B3
    await enterFormula(driver, 'B3', '2');

    // Enter formula '=(B1*B2)+(B3^2)' in cell B4
    await enterFormula(driver, 'B4', '=(B1*B2)+(B3^2)');

    // Verify cell B4 displays the result '19'
    const result = await getCellValue(driver, 'B4');
    expect(result).to.equal('19');
  });

  it('Formula referencing cells across sheets', async function() {
    // Add a new sheet
    const newSheetName = await addNewSheet(driver);

    // Enter value '100' in cell A1 of the new sheet
    await enterFormula(driver, 'A1', '100');

    // Switch back to the first sheet
    await driver.findElement(By.css('[data-testid="sheet1-tab"]')).click();

    // Enter formula '=Sheet2!A1 * 2' in cell C1
    await enterFormula(driver, 'C1', `=${newSheetName}!A1 * 2`);

    // Verify cell C1 displays the result '200'
    const result = await getCellValue(driver, 'C1');
    expect(result).to.equal('200');
  });

  it('Array formula calculation', async function() {
    // Enter values 1 to 5 in cells D1 to D5
    for (let i = 1; i <= 5; i++) {
      await enterFormula(driver, `D${i}`, i.toString());
    }

    // Enter array formula '=SUM(D1:D5*2)' in cell D6
    await enterFormula(driver, 'D6', '=SUM(D1:D5*2)');

    // Verify cell D6 displays the result '30'
    const result = await getCellValue(driver, 'D6');
    expect(result).to.equal('30');
  });

  it('Circular reference detection', async function() {
    // Enter formula '=E2' in cell E1
    await enterFormula(driver, 'E1', '=E2');

    // Try to enter formula '=E1' in cell E2
    await enterFormula(driver, 'E2', '=E1');

    // Verify that a circular reference error is displayed
    const errorMessage = await driver.findElement(By.css('[data-testid="error-message"]')).getText();
    expect(errorMessage).to.include('Circular reference detected');
  });
});

// Error handling and reporting for the test suite
process.on('unhandledRejection', async (error) => {
  console.error('Unhandled Promise Rejection:', error);
  
  // Capture screenshots on test failure
  if (driver) {
    const screenshot = await driver.takeScreenshot();
    const fileName = `error_screenshot_${Date.now()}.png`;
    require('fs').writeFileSync(fileName, screenshot, 'base64');
    console.log(`Screenshot saved as ${fileName}`);
  }

  // Log error messages and stack traces
  console.error('Error message:', error.message);
  console.error('Stack trace:', error.stack);

  // Attempt to close browser in case of unexpected errors
  if (driver) {
    try {
      await driver.quit();
    } catch (quitError) {
      console.error('Error while closing the browser:', quitError);
    }
  }

  process.exit(1);
});