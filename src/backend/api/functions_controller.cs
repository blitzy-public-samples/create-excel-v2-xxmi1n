using Microsoft.AspNetCore.Mvc;
using System.Threading.Tasks;
using System.Collections.Generic;
using ExcelServices;
using ExcelModels;
using ExcelAPI.Filters;

namespace ExcelAPI.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class FunctionsController : ControllerBase
    {
        private readonly FunctionService _functionService;
        private readonly ILogger<FunctionsController> _logger;

        public FunctionsController(FunctionService functionService, ILogger<FunctionsController> logger)
        {
            _functionService = functionService;
            _logger = logger;
        }

        [HttpPost("execute")]
        [ExcelAuthorize(Permissions.ExecuteFunctions)]
        public async Task<ActionResult<FunctionExecutionResult>> ExecuteFunction(FunctionExecutionRequest request)
        {
            _logger.LogInformation("Starting ExecuteFunction operation");

            // Validate the input request
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            try
            {
                // Execute the specified function
                var result = await _functionService.ExecuteFunctionAsync(request);
                _logger.LogInformation("ExecuteFunction operation completed successfully");
                return Ok(result);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during ExecuteFunction operation");
                return StatusCode(500, "An error occurred while executing the function");
            }
        }

        [HttpGet("available")]
        [ExcelAuthorize(Permissions.ViewFunctions)]
        public async Task<ActionResult<IEnumerable<string>>> GetAvailableFunctions()
        {
            _logger.LogInformation("Starting GetAvailableFunctions operation");

            try
            {
                // Retrieve the list of available functions
                var functions = await _functionService.GetAvailableFunctionsAsync();
                _logger.LogInformation("GetAvailableFunctions operation completed successfully");
                return Ok(functions);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error occurred during GetAvailableFunctions operation");
                return StatusCode(500, "An error occurred while retrieving available functions");
            }
        }

        [HttpGet("{functionName}/metadata")]
        [ExcelAuthorize(Permissions.ViewFunctions)]
        public async Task<ActionResult<FunctionMetadata>> GetFunctionMetadata(string functionName)
        {
            _logger.LogInformation($"Starting GetFunctionMetadata operation for function: {functionName}");

            try
            {
                // Retrieve the metadata for the specified function
                var metadata = await _functionService.GetFunctionMetadataAsync(functionName);

                if (metadata == null)
                {
                    _logger.LogWarning($"Function not found: {functionName}");
                    return NotFound($"Function '{functionName}' not found");
                }

                _logger.LogInformation($"GetFunctionMetadata operation completed successfully for function: {functionName}");
                return Ok(metadata);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error occurred during GetFunctionMetadata operation for function: {functionName}");
                return StatusCode(500, "An error occurred while retrieving function metadata");
            }
        }
    }
}