using NUnit.Framework;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using ExcelProject.Data;
using ExcelProject.Models;
using ExcelProject.Repositories;

namespace ExcelProject.Tests.Integration
{
    [TestFixture]
    public class DatabaseTests
    {
        private ExcelDbContext DbContext { get; set; }
        private UserRepository UserRepository { get; set; }
        private WorkbookRepository WorkbookRepository { get; set; }
        private WorksheetRepository WorksheetRepository { get; set; }

        [SetUp]
        public void SetUp()
        {
            // Set up in-memory database for testing
            var options = new DbContextOptionsBuilder<ExcelDbContext>()
                .UseInMemoryDatabase(databaseName: "TestExcelDb")
                .Options;
            DbContext = new ExcelDbContext(options);

            // Initialize repositories
            UserRepository = new UserRepository(DbContext);
            WorkbookRepository = new WorkbookRepository(DbContext);
            WorksheetRepository = new WorksheetRepository(DbContext);

            // Ensure database is created
            DbContext.Database.EnsureCreated();
        }

        [TearDown]
        public void TearDown()
        {
            // Clean up after each test
            DbContext.Database.EnsureDeleted();
            DbContext.Dispose();
        }

        [Test]
        public async Task CreateUser_SavesUserToDatabase()
        {
            // Arrange
            var user = new User { Username = "testuser", Email = "test@example.com" };

            // Act
            await UserRepository.AddAsync(user);
            await DbContext.SaveChangesAsync();

            // Assert
            var savedUser = await UserRepository.GetByIdAsync(user.Id);
            Assert.That(savedUser, Is.Not.Null);
            Assert.That(savedUser.Username, Is.EqualTo("testuser"));
            Assert.That(savedUser.Email, Is.EqualTo("test@example.com"));
        }

        [Test]
        public async Task CreateWorkbook_AssociatesWithUser()
        {
            // Arrange
            var user = new User { Username = "workbookuser", Email = "workbook@example.com" };
            await UserRepository.AddAsync(user);

            // Act
            var workbook = new Workbook { Name = "Test Workbook", OwnerId = user.Id };
            await WorkbookRepository.AddAsync(workbook);
            await DbContext.SaveChangesAsync();

            // Assert
            var savedWorkbook = await WorkbookRepository.GetByIdAsync(workbook.Id);
            Assert.That(savedWorkbook, Is.Not.Null);
            Assert.That(savedWorkbook.Name, Is.EqualTo("Test Workbook"));
            Assert.That(savedWorkbook.OwnerId, Is.EqualTo(user.Id));
        }

        [Test]
        public async Task UpdateWorksheet_UpdatesCorrectly()
        {
            // Arrange
            var workbook = new Workbook { Name = "Worksheet Test Workbook" };
            await WorkbookRepository.AddAsync(workbook);
            var worksheet = new Worksheet { Name = "Sheet1", WorkbookId = workbook.Id };
            await WorksheetRepository.AddAsync(worksheet);
            await DbContext.SaveChangesAsync();

            // Act
            worksheet.Name = "Updated Sheet";
            await WorksheetRepository.UpdateAsync(worksheet);
            await DbContext.SaveChangesAsync();

            // Assert
            var updatedWorksheet = await WorksheetRepository.GetByIdAsync(worksheet.Id);
            Assert.That(updatedWorksheet.Name, Is.EqualTo("Updated Sheet"));
        }

        [Test]
        public async Task DeleteWorkbook_CascadesCorrectly()
        {
            // Arrange
            var workbook = new Workbook { Name = "Cascade Test Workbook" };
            await WorkbookRepository.AddAsync(workbook);
            var worksheet = new Worksheet { Name = "Sheet1", WorkbookId = workbook.Id };
            await WorksheetRepository.AddAsync(worksheet);
            await DbContext.SaveChangesAsync();

            // Act
            await WorkbookRepository.DeleteAsync(workbook);
            await DbContext.SaveChangesAsync();

            // Assert
            var deletedWorkbook = await WorkbookRepository.GetByIdAsync(workbook.Id);
            var deletedWorksheet = await WorksheetRepository.GetByIdAsync(worksheet.Id);
            Assert.That(deletedWorkbook, Is.Null);
            Assert.That(deletedWorksheet, Is.Null);
        }
    }
}