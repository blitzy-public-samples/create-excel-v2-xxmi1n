using Microsoft.AspNetCore.Mvc;
using System.Threading.Tasks;
using System.Collections.Generic;
using ExcelServices;
using ExcelModels;
using ExcelAPI.Filters;
using Microsoft.Extensions.Logging;

namespace ExcelAPI.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class WorkbooksController : ControllerBase
    {
        private readonly WorkbookService _workbookService;
        private readonly ILogger<WorkbooksController> _logger;

        public WorkbooksController(WorkbookService workbookService, ILogger<WorkbooksController> logger)
        {
            _workbookService = workbookService;
            _logger = logger;
        }

        [HttpGet]
        [ExcelAuthorize(Permissions.ViewWorkbooks)]
        public async Task<ActionResult<IEnumerable<Workbook>>> GetWorkbooks()
        {
            _logger.LogInformation("Starting GetWorkbooks operation");
            try
            {
                var workbooks = await _workbookService.GetWorkbooksAsync();
                _logger.LogInformation("GetWorkbooks operation completed successfully");
                return Ok(workbooks);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during GetWorkbooks operation");
                return StatusCode(500, "An error occurred while retrieving workbooks");
            }
        }

        [HttpPost]
        [ExcelAuthorize(Permissions.CreateWorkbooks)]
        public async Task<ActionResult<Workbook>> CreateWorkbook([FromBody] WorkbookCreateModel model)
        {
            _logger.LogInformation("Starting CreateWorkbook operation");
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            try
            {
                var workbook = await _workbookService.CreateWorkbookAsync(model);
                _logger.LogInformation("CreateWorkbook operation completed successfully");
                return CreatedAtAction(nameof(GetWorkbook), new { id = workbook.Id }, workbook);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during CreateWorkbook operation");
                return StatusCode(500, "An error occurred while creating the workbook");
            }
        }

        [HttpGet("{id}")]
        [ExcelAuthorize(Permissions.ViewWorkbooks)]
        public async Task<ActionResult<Workbook>> GetWorkbook(string id)
        {
            _logger.LogInformation($"Starting GetWorkbook operation for id: {id}");
            try
            {
                var workbook = await _workbookService.GetWorkbookAsync(id);
                if (workbook == null)
                {
                    return NotFound();
                }
                _logger.LogInformation($"GetWorkbook operation completed successfully for id: {id}");
                return Ok(workbook);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during GetWorkbook operation for id: {id}");
                return StatusCode(500, "An error occurred while retrieving the workbook");
            }
        }

        [HttpPut("{id}")]
        [ExcelAuthorize(Permissions.EditWorkbooks)]
        public async Task<IActionResult> UpdateWorkbook(string id, [FromBody] WorkbookUpdateModel model)
        {
            _logger.LogInformation($"Starting UpdateWorkbook operation for id: {id}");
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            try
            {
                var result = await _workbookService.UpdateWorkbookAsync(id, model);
                if (!result)
                {
                    return NotFound();
                }
                _logger.LogInformation($"UpdateWorkbook operation completed successfully for id: {id}");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during UpdateWorkbook operation for id: {id}");
                return StatusCode(500, "An error occurred while updating the workbook");
            }
        }

        [HttpDelete("{id}")]
        [ExcelAuthorize(Permissions.DeleteWorkbooks)]
        public async Task<IActionResult> DeleteWorkbook(string id)
        {
            _logger.LogInformation($"Starting DeleteWorkbook operation for id: {id}");
            try
            {
                var result = await _workbookService.DeleteWorkbookAsync(id);
                if (!result)
                {
                    return NotFound();
                }
                _logger.LogInformation($"DeleteWorkbook operation completed successfully for id: {id}");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during DeleteWorkbook operation for id: {id}");
                return StatusCode(500, "An error occurred while deleting the workbook");
            }
        }
    }
}