using System.Threading.Tasks;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Extensions.Logging;
using ExcelModels;
using ExcelData;
using ExcelServices.Interfaces;

public class AuthorizationService : IAuthorizationService
{
    private readonly ILogger<AuthorizationService> _logger;
    private readonly IUserRepository _userRepository;
    private readonly IRoleRepository _roleRepository;
    private readonly IPermissionRepository _permissionRepository;

    public AuthorizationService(
        ILogger<AuthorizationService> logger,
        IUserRepository userRepository,
        IRoleRepository roleRepository,
        IPermissionRepository permissionRepository)
    {
        // Initialize dependencies
        _logger = logger;
        _userRepository = userRepository;
        _roleRepository = roleRepository;
        _permissionRepository = permissionRepository;
    }

    public async Task<bool> IsAuthorizedAsync(string userId, string action)
    {
        // Log the start of the authorization check
        _logger.LogInformation($"Checking authorization for user {userId} and action {action}");

        // Retrieve user permissions
        var userPermissions = await GetUserPermissionsAsync(userId);

        // Check if the required action is in the user's permissions
        var isAuthorized = userPermissions.Any(p => p.Name == action);

        // Log the result of the authorization check
        _logger.LogInformation($"Authorization result for user {userId} and action {action}: {isAuthorized}");

        // Return the authorization result
        return isAuthorized;
    }

    private async Task<IEnumerable<Permission>> GetUserPermissionsAsync(string userId)
    {
        // Retrieve the user with roles
        var user = await _userRepository.GetUserWithRolesAsync(userId);

        // If user is not found, return an empty list of permissions
        if (user == null)
        {
            _logger.LogWarning($"User with ID {userId} not found");
            return Enumerable.Empty<Permission>();
        }

        // Initialize a set to store unique permissions
        var permissions = new HashSet<Permission>();

        // For each role the user has, retrieve and add the permissions
        foreach (var role in user.Roles)
        {
            var rolePermissions = await _roleRepository.GetRolePermissionsAsync(role.Id);
            foreach (var permission in rolePermissions)
            {
                permissions.Add(permission);
            }
        }

        // Return the set of permissions as an IEnumerable<Permission>
        return permissions;
    }

    public async Task<bool> AddUserToRoleAsync(string userId, string roleName)
    {
        // Retrieve the user
        var user = await _userRepository.GetUserAsync(userId);
        if (user == null)
        {
            _logger.LogWarning($"User with ID {userId} not found");
            return false;
        }

        // Retrieve the role
        var role = await _roleRepository.GetRoleByNameAsync(roleName);
        if (role == null)
        {
            _logger.LogWarning($"Role with name {roleName} not found");
            return false;
        }

        // Add the role to the user's roles if not already present
        if (!user.Roles.Any(r => r.Id == role.Id))
        {
            user.Roles.Add(role);
            await _userRepository.UpdateUserAsync(user);
            _logger.LogInformation($"Added user {userId} to role {roleName}");
            return true;
        }

        _logger.LogInformation($"User {userId} is already in role {roleName}");
        return false;
    }

    public async Task<bool> RemoveUserFromRoleAsync(string userId, string roleName)
    {
        // Retrieve the user
        var user = await _userRepository.GetUserAsync(userId);
        if (user == null)
        {
            _logger.LogWarning($"User with ID {userId} not found");
            return false;
        }

        // Retrieve the role
        var role = await _roleRepository.GetRoleByNameAsync(roleName);
        if (role == null)
        {
            _logger.LogWarning($"Role with name {roleName} not found");
            return false;
        }

        // Remove the role from the user's roles if present
        var roleToRemove = user.Roles.FirstOrDefault(r => r.Id == role.Id);
        if (roleToRemove != null)
        {
            user.Roles.Remove(roleToRemove);
            await _userRepository.UpdateUserAsync(user);
            _logger.LogInformation($"Removed user {userId} from role {roleName}");
            return true;
        }

        _logger.LogInformation($"User {userId} is not in role {roleName}");
        return false;
    }
}