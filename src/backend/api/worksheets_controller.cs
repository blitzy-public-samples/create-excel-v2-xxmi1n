using Microsoft.AspNetCore.Mvc;
using System.Threading.Tasks;
using System.Collections.Generic;
using ExcelServices;
using ExcelModels;
using ExcelAPI.Filters;

namespace ExcelAPI.Controllers
{
    [ApiController]
    [Route("api/workbooks/{workbookId}/[controller]")]
    public class WorksheetsController : ControllerBase
    {
        private readonly WorksheetService _worksheetService;
        private readonly ILogger<WorksheetsController> _logger;

        public WorksheetsController(WorksheetService worksheetService, ILogger<WorksheetsController> logger)
        {
            _worksheetService = worksheetService;
            _logger = logger;
        }

        [HttpGet]
        [ExcelAuthorize(Permissions.ViewWorksheets)]
        public async Task<ActionResult<IEnumerable<Worksheet>>> GetWorksheets(string workbookId)
        {
            _logger.LogInformation($"Starting GetWorksheets operation for workbook {workbookId}");
            try
            {
                var worksheets = await _worksheetService.GetWorksheetsAsync(workbookId);
                _logger.LogInformation($"GetWorksheets operation completed successfully for workbook {workbookId}");
                return Ok(worksheets);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during GetWorksheets operation for workbook {workbookId}");
                return StatusCode(500, "An error occurred while processing your request.");
            }
        }

        [HttpPost]
        [ExcelAuthorize(Permissions.CreateWorksheets)]
        public async Task<ActionResult<Worksheet>> CreateWorksheet(string workbookId, WorksheetCreateModel model)
        {
            _logger.LogInformation($"Starting CreateWorksheet operation for workbook {workbookId}");
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            try
            {
                var worksheet = await _worksheetService.CreateWorksheetAsync(workbookId, model);
                _logger.LogInformation($"CreateWorksheet operation completed successfully for workbook {workbookId}");
                return CreatedAtAction(nameof(GetWorksheet), new { workbookId, id = worksheet.Id }, worksheet);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during CreateWorksheet operation for workbook {workbookId}");
                return StatusCode(500, "An error occurred while processing your request.");
            }
        }

        [HttpGet("{id}")]
        [ExcelAuthorize(Permissions.ViewWorksheets)]
        public async Task<ActionResult<Worksheet>> GetWorksheet(string workbookId, string id)
        {
            _logger.LogInformation($"Starting GetWorksheet operation for workbook {workbookId}, worksheet {id}");
            try
            {
                var worksheet = await _worksheetService.GetWorksheetAsync(workbookId, id);
                if (worksheet == null)
                {
                    return NotFound();
                }
                _logger.LogInformation($"GetWorksheet operation completed successfully for workbook {workbookId}, worksheet {id}");
                return Ok(worksheet);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during GetWorksheet operation for workbook {workbookId}, worksheet {id}");
                return StatusCode(500, "An error occurred while processing your request.");
            }
        }

        [HttpPut("{id}")]
        [ExcelAuthorize(Permissions.EditWorksheets)]
        public async Task<IActionResult> UpdateWorksheet(string workbookId, string id, WorksheetUpdateModel model)
        {
            _logger.LogInformation($"Starting UpdateWorksheet operation for workbook {workbookId}, worksheet {id}");
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            try
            {
                var result = await _worksheetService.UpdateWorksheetAsync(workbookId, id, model);
                if (!result)
                {
                    return NotFound();
                }
                _logger.LogInformation($"UpdateWorksheet operation completed successfully for workbook {workbookId}, worksheet {id}");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during UpdateWorksheet operation for workbook {workbookId}, worksheet {id}");
                return StatusCode(500, "An error occurred while processing your request.");
            }
        }

        [HttpDelete("{id}")]
        [ExcelAuthorize(Permissions.DeleteWorksheets)]
        public async Task<IActionResult> DeleteWorksheet(string workbookId, string id)
        {
            _logger.LogInformation($"Starting DeleteWorksheet operation for workbook {workbookId}, worksheet {id}");
            try
            {
                var result = await _worksheetService.DeleteWorksheetAsync(workbookId, id);
                if (!result)
                {
                    return NotFound();
                }
                _logger.LogInformation($"DeleteWorksheet operation completed successfully for workbook {workbookId}, worksheet {id}");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during DeleteWorksheet operation for workbook {workbookId}, worksheet {id}");
                return StatusCode(500, "An error occurred while processing your request.");
            }
        }
    }
}