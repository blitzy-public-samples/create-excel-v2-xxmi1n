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
    public class PivotTablesController : ControllerBase
    {
        private readonly PivotTableService _pivotTableService;
        private readonly ILogger<PivotTablesController> _logger;

        public PivotTablesController(PivotTableService pivotTableService, ILogger<PivotTablesController> logger)
        {
            _pivotTableService = pivotTableService;
            _logger = logger;
        }

        [HttpGet]
        [ExcelAuthorize(Permissions.ViewPivotTables)]
        public async Task<ActionResult<IEnumerable<PivotTable>>> GetPivotTables(string workbookId, string worksheetId)
        {
            _logger.LogInformation("Starting GetPivotTables operation");
            try
            {
                var pivotTables = await _pivotTableService.GetPivotTablesAsync(workbookId, worksheetId);
                _logger.LogInformation("GetPivotTables operation completed successfully");
                return Ok(pivotTables);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during GetPivotTables operation");
                return StatusCode(500, "An error occurred while retrieving pivot tables");
            }
        }

        [HttpGet("{pivotTableId}")]
        [ExcelAuthorize(Permissions.ViewPivotTables)]
        public async Task<ActionResult<PivotTable>> GetPivotTable(string workbookId, string worksheetId, string pivotTableId)
        {
            _logger.LogInformation("Starting GetPivotTable operation");
            try
            {
                var pivotTable = await _pivotTableService.GetPivotTableAsync(workbookId, worksheetId, pivotTableId);
                if (pivotTable == null)
                {
                    return NotFound();
                }
                _logger.LogInformation("GetPivotTable operation completed successfully");
                return Ok(pivotTable);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during GetPivotTable operation");
                return StatusCode(500, "An error occurred while retrieving the pivot table");
            }
        }

        [HttpPost]
        [ExcelAuthorize(Permissions.CreatePivotTables)]
        public async Task<ActionResult<PivotTable>> CreatePivotTable(string workbookId, string worksheetId, PivotTableCreateModel model)
        {
            _logger.LogInformation("Starting CreatePivotTable operation");
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            try
            {
                var newPivotTable = await _pivotTableService.CreatePivotTableAsync(workbookId, worksheetId, model);
                _logger.LogInformation("CreatePivotTable operation completed successfully");
                return CreatedAtAction(nameof(GetPivotTable), new { workbookId, worksheetId, pivotTableId = newPivotTable.Id }, newPivotTable);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during CreatePivotTable operation");
                return StatusCode(500, "An error occurred while creating the pivot table");
            }
        }

        [HttpPut("{pivotTableId}")]
        [ExcelAuthorize(Permissions.EditPivotTables)]
        public async Task<IActionResult> UpdatePivotTable(string workbookId, string worksheetId, string pivotTableId, PivotTableUpdateModel model)
        {
            _logger.LogInformation("Starting UpdatePivotTable operation");
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            try
            {
                var result = await _pivotTableService.UpdatePivotTableAsync(workbookId, worksheetId, pivotTableId, model);
                if (!result)
                {
                    return NotFound();
                }
                _logger.LogInformation("UpdatePivotTable operation completed successfully");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during UpdatePivotTable operation");
                return StatusCode(500, "An error occurred while updating the pivot table");
            }
        }

        [HttpDelete("{pivotTableId}")]
        [ExcelAuthorize(Permissions.DeletePivotTables)]
        public async Task<IActionResult> DeletePivotTable(string workbookId, string worksheetId, string pivotTableId)
        {
            _logger.LogInformation("Starting DeletePivotTable operation");
            try
            {
                var result = await _pivotTableService.DeletePivotTableAsync(workbookId, worksheetId, pivotTableId);
                if (!result)
                {
                    return NotFound();
                }
                _logger.LogInformation("DeletePivotTable operation completed successfully");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during DeletePivotTable operation");
                return StatusCode(500, "An error occurred while deleting the pivot table");
            }
        }

        [HttpPost("{pivotTableId}/refresh")]
        [ExcelAuthorize(Permissions.EditPivotTables)]
        public async Task<IActionResult> RefreshPivotTable(string workbookId, string worksheetId, string pivotTableId)
        {
            _logger.LogInformation("Starting RefreshPivotTable operation");
            try
            {
                var result = await _pivotTableService.RefreshPivotTableAsync(workbookId, worksheetId, pivotTableId);
                if (!result)
                {
                    return NotFound();
                }
                _logger.LogInformation("RefreshPivotTable operation completed successfully");
                return Ok();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during RefreshPivotTable operation");
                return StatusCode(500, "An error occurred while refreshing the pivot table");
            }
        }
    }
}