using NUnit.Framework;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc.Testing;
using System.Net.Http;
using Newtonsoft.Json;
using ExcelProject.Api;
using ExcelProject.Models;

namespace ExcelProject.Tests.Integration
{
    [TestFixture]
    public class ApiTests
    {
        private WebApplicationFactory<Startup> Factory { get; set; }
        private HttpClient Client { get; set; }

        [SetUp]
        public void SetUp()
        {
            // Set up the test environment before each test
            Factory = new WebApplicationFactory<Startup>();
            Client = Factory.CreateClient();
        }

        [TearDown]
        public void TearDown()
        {
            // Clean up the test environment after each test
            Client.Dispose();
            Factory.Dispose();
        }

        [Test]
        public async Task CreateWorkbook_ReturnsCreatedWorkbook()
        {
            // Test the creation of a new workbook via API
            var response = await Client.PostAsync("/api/workbooks", new StringContent(JsonConvert.SerializeObject(new { Name = "Test Workbook" })));
            response.EnsureSuccessStatusCode();
            var content = await response.Content.ReadAsStringAsync();
            var workbook = JsonConvert.DeserializeObject<Workbook>(content);
            Assert.That(workbook, Is.Not.Null);
            Assert.That(workbook.Name, Is.EqualTo("Test Workbook"));
        }

        [Test]
        public async Task GetWorkbook_ReturnsCorrectWorkbook()
        {
            // Test retrieving a workbook by ID
            var createResponse = await Client.PostAsync("/api/workbooks", new StringContent(JsonConvert.SerializeObject(new { Name = "Test Workbook" })));
            var createdWorkbook = JsonConvert.DeserializeObject<Workbook>(await createResponse.Content.ReadAsStringAsync());
            var getResponse = await Client.GetAsync($"/api/workbooks/{createdWorkbook.Id}");
            getResponse.EnsureSuccessStatusCode();
            var content = await getResponse.Content.ReadAsStringAsync();
            var retrievedWorkbook = JsonConvert.DeserializeObject<Workbook>(content);
            Assert.That(retrievedWorkbook.Id, Is.EqualTo(createdWorkbook.Id));
            Assert.That(retrievedWorkbook.Name, Is.EqualTo("Test Workbook"));
        }

        [Test]
        public async Task UpdateWorksheet_UpdatesCorrectly()
        {
            // Test updating a worksheet in a workbook
            var workbook = await CreateTestWorkbook();
            var worksheet = workbook.Worksheets[0];
            var updateResponse = await Client.PutAsync($"/api/workbooks/{workbook.Id}/worksheets/{worksheet.Id}", new StringContent(JsonConvert.SerializeObject(new { Name = "Updated Sheet" })));
            updateResponse.EnsureSuccessStatusCode();
            var getResponse = await Client.GetAsync($"/api/workbooks/{workbook.Id}/worksheets/{worksheet.Id}");
            var updatedWorksheet = JsonConvert.DeserializeObject<Worksheet>(await getResponse.Content.ReadAsStringAsync());
            Assert.That(updatedWorksheet.Name, Is.EqualTo("Updated Sheet"));
        }

        [Test]
        public async Task SetCellValue_UpdatesCellCorrectly()
        {
            // Test setting a cell value via API
            var workbook = await CreateTestWorkbook();
            var worksheet = workbook.Worksheets[0];
            var cellUpdate = new { Value = "Test Value" };
            var updateResponse = await Client.PutAsync($"/api/workbooks/{workbook.Id}/worksheets/{worksheet.Id}/cells/A1", new StringContent(JsonConvert.SerializeObject(cellUpdate)));
            updateResponse.EnsureSuccessStatusCode();
            var getResponse = await Client.GetAsync($"/api/workbooks/{workbook.Id}/worksheets/{worksheet.Id}/cells/A1");
            var updatedCell = JsonConvert.DeserializeObject<Cell>(await getResponse.Content.ReadAsStringAsync());
            Assert.That(updatedCell.Value, Is.EqualTo("Test Value"));
        }

        private async Task<Workbook> CreateTestWorkbook()
        {
            // Helper method to create a test workbook
            var response = await Client.PostAsync("/api/workbooks", new StringContent(JsonConvert.SerializeObject(new { Name = "Test Workbook" })));
            response.EnsureSuccessStatusCode();
            return JsonConvert.DeserializeObject<Workbook>(await response.Content.ReadAsStringAsync());
        }
    }
}