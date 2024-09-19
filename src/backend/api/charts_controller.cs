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
    [Route("api/workbooks/{workbookId}/worksheets/{worksheetId}/[controller]")]
    public class ChartsController : ControllerBase
    {
        private readonly ChartService _chartService;
        private readonly ILogger<ChartsController> _logger;

        public ChartsController(ChartService chartService, ILogger<ChartsController> logger)
        {
            _chartService = chartService;
            _logger = logger;
        }

        [HttpGet]
        [ExcelAuthorize(Permissions.ViewCharts)]
        public async Task<ActionResult<IEnumerable<Chart>>> GetCharts(string workbookId, string worksheetId)
        {
            _logger.LogInformation("Starting GetCharts operation");
            try
            {
                // Retrieve all charts in the worksheet
                var charts = await _chartService.GetChartsAsync(workbookId, worksheetId);
                _logger.LogInformation("GetCharts operation completed successfully");
                return Ok(charts);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during GetCharts operation");
                return StatusCode(500, "An error occurred while retrieving charts");
            }
        }

        [HttpGet("{chartId}")]
        [ExcelAuthorize(Permissions.ViewCharts)]
        public async Task<ActionResult<Chart>> GetChart(string workbookId, string worksheetId, string chartId)
        {
            _logger.LogInformation("Starting GetChart operation");
            try
            {
                // Retrieve the specified chart
                var chart = await _chartService.GetChartAsync(workbookId, worksheetId, chartId);
                if (chart == null)
                {
                    _logger.LogInformation("Chart not found");
                    return NotFound();
                }
                _logger.LogInformation("GetChart operation completed successfully");
                return Ok(chart);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during GetChart operation");
                return StatusCode(500, "An error occurred while retrieving the chart");
            }
        }

        [HttpPost]
        [ExcelAuthorize(Permissions.CreateCharts)]
        public async Task<ActionResult<Chart>> CreateChart(string workbookId, string worksheetId, ChartCreateModel model)
        {
            _logger.LogInformation("Starting CreateChart operation");
            try
            {
                // Validate the input model
                if (!ModelState.IsValid)
                {
                    return BadRequest(ModelState);
                }

                // Create the new chart
                var newChart = await _chartService.CreateChartAsync(workbookId, worksheetId, model);
                _logger.LogInformation("CreateChart operation completed successfully");
                return CreatedAtAction(nameof(GetChart), new { workbookId, worksheetId, chartId = newChart.Id }, newChart);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during CreateChart operation");
                return StatusCode(500, "An error occurred while creating the chart");
            }
        }

        [HttpPut("{chartId}")]
        [ExcelAuthorize(Permissions.EditCharts)]
        public async Task<IActionResult> UpdateChart(string workbookId, string worksheetId, string chartId, ChartUpdateModel model)
        {
            _logger.LogInformation("Starting UpdateChart operation");
            try
            {
                // Validate the input model
                if (!ModelState.IsValid)
                {
                    return BadRequest(ModelState);
                }

                // Update the chart
                var result = await _chartService.UpdateChartAsync(workbookId, worksheetId, chartId, model);
                if (!result)
                {
                    _logger.LogInformation("Chart not found");
                    return NotFound();
                }
                _logger.LogInformation("UpdateChart operation completed successfully");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during UpdateChart operation");
                return StatusCode(500, "An error occurred while updating the chart");
            }
        }

        [HttpDelete("{chartId}")]
        [ExcelAuthorize(Permissions.DeleteCharts)]
        public async Task<IActionResult> DeleteChart(string workbookId, string worksheetId, string chartId)
        {
            _logger.LogInformation("Starting DeleteChart operation");
            try
            {
                // Delete the specified chart
                var result = await _chartService.DeleteChartAsync(workbookId, worksheetId, chartId);
                if (!result)
                {
                    _logger.LogInformation("Chart not found");
                    return NotFound();
                }
                _logger.LogInformation("DeleteChart operation completed successfully");
                return NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during DeleteChart operation");
                return StatusCode(500, "An error occurred while deleting the chart");
            }
        }
    }
}