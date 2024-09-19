using Microsoft.AspNetCore.Mvc;
using System.Threading.Tasks;
using ExcelServices;
using ExcelModels;
using ExcelAPI.Filters;
using Microsoft.Extensions.Logging;

namespace ExcelAPI.Controllers
{
    [ApiController]
    [Route("api/workbooks/{workbookId}/worksheets/{worksheetId}/[controller]")]
    public class RangesController : ControllerBase
    {
        private readonly RangeService _rangeService;
        private readonly ILogger<RangesController> _logger;

        public RangesController(RangeService rangeService, ILogger<RangesController> logger)
        {
            _rangeService = rangeService;
            _logger = logger;
        }

        [HttpGet("{address}")]
        [ExcelAuthorize(Permissions.ViewRanges)]
        public async Task<ActionResult<Range>> GetRange(string workbookId, string worksheetId, string address)
        {
            _logger.LogInformation($"Starting GetRange operation for workbook: {workbookId}, worksheet: {worksheetId}, address: {address}");

            try
            {
                var range = await _rangeService.GetRangeAsync(workbookId, worksheetId, address);
                if (range == null)
                {
                    return NotFound();
                }

                _logger.LogInformation($"GetRange operation completed successfully");
                return Ok(range);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during GetRange operation");
                return StatusCode(500, "An error occurred while processing your request.");
            }
        }

        [HttpPut("{address}")]
        [ExcelAuthorize(Permissions.EditRanges)]
        public async Task<IActionResult> UpdateRange(string workbookId, string worksheetId, string address, RangeUpdateModel model)
        {
            _logger.LogInformation($"Starting UpdateRange operation for workbook: {workbookId}, worksheet: {worksheetId}, address: {address}");

            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            try
            {
                var result = await _rangeService.UpdateRangeAsync(workbookId, worksheetId, address, model);
                if (!result)
                {
                    return NotFound();
                }

                _logger.LogInformation($"UpdateRange operation completed successfully");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during UpdateRange operation");
                return StatusCode(500, "An error occurred while processing your request.");
            }
        }

        [HttpPost("{address}/values")]
        [ExcelAuthorize(Permissions.EditRanges)]
        public async Task<IActionResult> SetRangeValues(string workbookId, string worksheetId, string address, RangeValueModel model)
        {
            _logger.LogInformation($"Starting SetRangeValues operation for workbook: {workbookId}, worksheet: {worksheetId}, address: {address}");

            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            try
            {
                var result = await _rangeService.SetRangeValuesAsync(workbookId, worksheetId, address, model);
                if (!result)
                {
                    return NotFound();
                }

                _logger.LogInformation($"SetRangeValues operation completed successfully");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during SetRangeValues operation");
                return StatusCode(500, "An error occurred while processing your request.");
            }
        }

        [HttpPost("{address}/clear")]
        [ExcelAuthorize(Permissions.EditRanges)]
        public async Task<IActionResult> ClearRange(string workbookId, string worksheetId, string address)
        {
            _logger.LogInformation($"Starting ClearRange operation for workbook: {workbookId}, worksheet: {worksheetId}, address: {address}");

            try
            {
                var result = await _rangeService.ClearRangeAsync(workbookId, worksheetId, address);
                if (!result)
                {
                    return NotFound();
                }

                _logger.LogInformation($"ClearRange operation completed successfully");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during ClearRange operation");
                return StatusCode(500, "An error occurred while processing your request.");
            }
        }
    }
}