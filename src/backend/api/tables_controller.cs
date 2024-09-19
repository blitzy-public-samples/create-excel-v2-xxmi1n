using Microsoft.AspNetCore.Mvc;
using System.Threading.Tasks;
using System.Collections.Generic;
using ExcelServices;
using ExcelModels;
using ExcelAPI.Filters;

namespace ExcelAPI.Controllers
{
    [ApiController]
    [Route("api/workbooks/{workbookId}/worksheets/{worksheetId}/[controller]")]
    public class TablesController : ControllerBase
    {
        private readonly TableService _tableService;
        private readonly ILogger<TablesController> _logger;

        public TablesController(TableService tableService, ILogger<TablesController> logger)
        {
            _tableService = tableService;
            _logger = logger;
        }

        [HttpGet]
        [ExcelAuthorize(Permissions.ViewTables)]
        public async Task<ActionResult<IEnumerable<Table>>> GetTables(string workbookId, string worksheetId)
        {
            _logger.LogInformation("Starting GetTables operation for workbook {WorkbookId} and worksheet {WorksheetId}", workbookId, worksheetId);
            try
            {
                var tables = await _tableService.GetTablesAsync(workbookId, worksheetId);
                _logger.LogInformation("GetTables operation completed successfully");
                return Ok(tables);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during GetTables operation");
                return StatusCode(500, "An error occurred while retrieving tables");
            }
        }

        [HttpGet("{tableId}")]
        [ExcelAuthorize(Permissions.ViewTables)]
        public async Task<ActionResult<Table>> GetTable(string workbookId, string worksheetId, string tableId)
        {
            _logger.LogInformation("Starting GetTable operation for table {TableId}", tableId);
            try
            {
                var table = await _tableService.GetTableAsync(workbookId, worksheetId, tableId);
                if (table == null)
                {
                    _logger.LogWarning("Table {TableId} not found", tableId);
                    return NotFound();
                }
                _logger.LogInformation("GetTable operation completed successfully");
                return Ok(table);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during GetTable operation");
                return StatusCode(500, "An error occurred while retrieving the table");
            }
        }

        [HttpPost]
        [ExcelAuthorize(Permissions.CreateTables)]
        public async Task<ActionResult<Table>> CreateTable(string workbookId, string worksheetId, TableCreateModel model)
        {
            _logger.LogInformation("Starting CreateTable operation");
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            try
            {
                var newTable = await _tableService.CreateTableAsync(workbookId, worksheetId, model);
                _logger.LogInformation("CreateTable operation completed successfully");
                return CreatedAtAction(nameof(GetTable), new { workbookId, worksheetId, tableId = newTable.Id }, newTable);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during CreateTable operation");
                return StatusCode(500, "An error occurred while creating the table");
            }
        }

        [HttpPut("{tableId}")]
        [ExcelAuthorize(Permissions.EditTables)]
        public async Task<IActionResult> UpdateTable(string workbookId, string worksheetId, string tableId, TableUpdateModel model)
        {
            _logger.LogInformation("Starting UpdateTable operation for table {TableId}", tableId);
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            try
            {
                var result = await _tableService.UpdateTableAsync(workbookId, worksheetId, tableId, model);
                if (!result)
                {
                    _logger.LogWarning("Table {TableId} not found for update", tableId);
                    return NotFound();
                }
                _logger.LogInformation("UpdateTable operation completed successfully");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during UpdateTable operation");
                return StatusCode(500, "An error occurred while updating the table");
            }
        }

        [HttpDelete("{tableId}")]
        [ExcelAuthorize(Permissions.DeleteTables)]
        public async Task<IActionResult> DeleteTable(string workbookId, string worksheetId, string tableId)
        {
            _logger.LogInformation("Starting DeleteTable operation for table {TableId}", tableId);
            try
            {
                var result = await _tableService.DeleteTableAsync(workbookId, worksheetId, tableId);
                if (!result)
                {
                    _logger.LogWarning("Table {TableId} not found for deletion", tableId);
                    return NotFound();
                }
                _logger.LogInformation("DeleteTable operation completed successfully");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during DeleteTable operation");
                return StatusCode(500, "An error occurred while deleting the table");
            }
        }
    }
}