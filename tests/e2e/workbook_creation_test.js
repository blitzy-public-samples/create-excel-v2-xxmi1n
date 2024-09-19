const { By, until } = require('selenium-webdriver');
const { Driver } = require('selenium-webdriver/chrome');
const { expect } = require('chai');
const { initializeDriver, waitForElementVisible, waitForElementClickable } = require('../utils/test_helper');

const BASE_URL = 'https://excel.example.com';
const DEFAULT_TIMEOUT = 10000;

// Function to create a new workbook
async function createNewWorkbook(driver) {
    // Click on 'New Workbook' button
    const newWorkbookButton = await driver.findElement(By.id('new-workbook-button'));
    await newWorkbookButton.click();

    // Wait for the workbook to load
    await driver.wait(until.elementLocated(By.id('workbook-title')), DEFAULT_TIMEOUT);

    // Return the workbook title
    const workbookTitle = await driver.findElement(By.id('workbook-title')).getText();
    return workbookTitle;
}

// Function to enter a value into a specific cell
async function enterCellValue(driver, cellId, value) {
    // Locate the cell by ID
    const cell = await driver.findElement(By.id(cellId));

    // Click on the cell
    await cell.click();

    // Enter the value
    await cell.sendKeys(value);

    // Press Enter to confirm
    await cell.sendKeys(Driver.Key.ENTER);
}

// Function to save the current workbook
async function saveWorkbook(driver, workbookName) {
    // Click on 'File' menu
    const fileMenu = await driver.findElement(By.id('file-menu'));
    await fileMenu.click();

    // Click on 'Save As'
    const saveAsButton = await driver.findElement(By.id('save-as-button'));
    await saveAsButton.click();

    // Enter the workbook name
    const nameInput = await driver.findElement(By.id('workbook-name-input'));
    await nameInput.sendKeys(workbookName);

    // Click 'Save' button
    const saveButton = await driver.findElement(By.id('save-button'));
    await saveButton.click();

    // Wait for save confirmation
    await driver.wait(until.elementLocated(By.id('save-confirmation')), DEFAULT_TIMEOUT);
}

describe('Workbook Creation and Saving', function() {
    let driver;

    before(async function() {
        // Initialize the WebDriver
        driver = await initializeDriver();

        // Navigate to the Excel web application
        await driver.get(BASE_URL);
    });

    after(async function() {
        // Close the browser
        if (driver) {
            await driver.quit();
        }
    });

    it('Create and save a new workbook', async function() {
        // Create a new workbook
        const workbookTitle = await createNewWorkbook(driver);
        expect(workbookTitle).to.not.be.empty;

        // Enter value '10' in cell A1
        await enterCellValue(driver, 'cell-A1', '10');

        // Enter value '20' in cell A2
        await enterCellValue(driver, 'cell-A2', '20');

        // Enter formula '=SUM(A1:A2)' in cell A3
        await enterCellValue(driver, 'cell-A3', '=SUM(A1:A2)');

        // Save the workbook as 'TestWorkbook'
        await saveWorkbook(driver, 'TestWorkbook');

        // Verify the workbook is saved successfully
        const saveConfirmation = await driver.findElement(By.id('save-confirmation')).getText();
        expect(saveConfirmation).to.include('TestWorkbook');
    });

    it('Create workbook with multiple sheets', async function() {
        // Create a new workbook
        await createNewWorkbook(driver);

        // Add a new sheet
        const addSheetButton = await driver.findElement(By.id('add-sheet-button'));
        await addSheetButton.click();

        // Enter data in both sheets
        await enterCellValue(driver, 'sheet1-cell-A1', 'Sheet 1 Data');
        await driver.findElement(By.id('sheet2-tab')).click();
        await enterCellValue(driver, 'sheet2-cell-A1', 'Sheet 2 Data');

        // Save the workbook
        await saveWorkbook(driver, 'MultiSheetWorkbook');

        // Verify all sheets are saved correctly
        await driver.findElement(By.id('sheet1-tab')).click();
        const sheet1Data = await driver.findElement(By.id('sheet1-cell-A1')).getText();
        expect(sheet1Data).to.equal('Sheet 1 Data');

        await driver.findElement(By.id('sheet2-tab')).click();
        const sheet2Data = await driver.findElement(By.id('sheet2-cell-A1')).getText();
        expect(sheet2Data).to.equal('Sheet 2 Data');
    });

    it('Create workbook with formatting', async function() {
        // Create a new workbook
        await createNewWorkbook(driver);

        // Enter data in cells
        await enterCellValue(driver, 'cell-A1', 'Formatted Cell');

        // Apply bold formatting to a cell
        const boldButton = await driver.findElement(By.id('bold-button'));
        await boldButton.click();

        // Apply color to a cell
        const colorPicker = await driver.findElement(By.id('color-picker'));
        await colorPicker.click();
        const redColor = await driver.findElement(By.id('red-color'));
        await redColor.click();

        // Save the workbook
        await saveWorkbook(driver, 'FormattedWorkbook');

        // Verify formatting is preserved after save
        const formattedCell = await driver.findElement(By.id('cell-A1'));
        const isBold = await formattedCell.getCssValue('font-weight');
        const color = await formattedCell.getCssValue('color');

        expect(isBold).to.equal('700'); // 700 represents bold
        expect(color).to.include('rgb(255, 0, 0)'); // Red color
    });
});

// Error handling and reporting for the test suite
process.on('unhandledRejection', async (error) => {
    console.error('Unhandled Promise Rejection:', error);
    
    // Capture screenshots on test failure
    if (driver) {
        const screenshot = await driver.takeScreenshot();
        // Save screenshot logic here
    }

    // Log error messages and stack traces
    console.error('Error message:', error.message);
    console.error('Stack trace:', error.stack);

    // Attempt to close browser in case of unexpected errors
    if (driver) {
        await driver.quit();
    }

    process.exit(1);
});