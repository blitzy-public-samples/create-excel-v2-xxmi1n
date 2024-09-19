using System;
using System.IdentityModel.Tokens.Jwt;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;
using Microsoft.IdentityModel.Tokens;
using ExcelModels;
using ExcelData;
using ExcelServices.Interfaces;
using System.Security.Claims;
using System.Text;
using System.Threading.Tasks;

namespace ExcelServices
{
    public class AuthenticationService : IAuthenticationService
    {
        private const string JWT_SECRET_KEY = "JwtSettings:SecretKey";
        private const string JWT_ISSUER = "JwtSettings:Issuer";
        private const string JWT_AUDIENCE = "JwtSettings:Audience";
        private const int JWT_EXPIRATION_MINUTES = 60;

        private readonly IConfiguration _configuration;
        private readonly ILogger<AuthenticationService> _logger;
        private readonly IUserRepository _userRepository;

        public AuthenticationService(IConfiguration configuration, ILogger<AuthenticationService> logger, IUserRepository userRepository)
        {
            _configuration = configuration;
            _logger = logger;
            _userRepository = userRepository;
        }

        public async Task<AuthenticationResult> AuthenticateUserAsync(string username, string password)
        {
            _logger.LogInformation($"Attempting to authenticate user: {username}");

            // Retrieve the user from the repository
            var user = await _userRepository.GetUserByUsernameAsync(username);

            if (user == null)
            {
                _logger.LogWarning($"User not found: {username}");
                return new AuthenticationResult { Success = false, Message = "Invalid username or password" };
            }

            // Verify the password
            if (!await VerifyPasswordAsync(user, password))
            {
                _logger.LogWarning($"Invalid password for user: {username}");
                return new AuthenticationResult { Success = false, Message = "Invalid username or password" };
            }

            // Generate JWT token
            var token = await GenerateJwtTokenAsync(user);

            _logger.LogInformation($"User authenticated successfully: {username}");
            return new AuthenticationResult { Success = true, Token = token };
        }

        private async Task<bool> VerifyPasswordAsync(User user, string password)
        {
            // TODO: Implement secure password hashing and verification
            // For demonstration purposes, we're using a simple comparison
            // In a real-world scenario, use a secure password hashing library
            return await Task.FromResult(user.PasswordHash == password);
        }

        private async Task<string> GenerateJwtTokenAsync(User user)
        {
            var secretKey = _configuration[JWT_SECRET_KEY];
            var issuer = _configuration[JWT_ISSUER];
            var audience = _configuration[JWT_AUDIENCE];

            var securityKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(secretKey));
            var credentials = new SigningCredentials(securityKey, SecurityAlgorithms.HmacSha256);

            var claims = new[]
            {
                new Claim(ClaimTypes.NameIdentifier, user.Id.ToString()),
                new Claim(ClaimTypes.Name, user.Username),
                new Claim(ClaimTypes.Role, user.Role)
            };

            var token = new JwtSecurityToken(
                issuer: issuer,
                audience: audience,
                claims: claims,
                expires: DateTime.UtcNow.AddMinutes(JWT_EXPIRATION_MINUTES),
                signingCredentials: credentials
            );

            return await Task.FromResult(new JwtSecurityTokenHandler().WriteToken(token));
        }
    }
}