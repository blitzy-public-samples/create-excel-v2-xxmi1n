using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;
using ExcelServices.Interfaces;
using ExcelModels;

namespace ExcelServices
{
    public class DLPService : IDLPService
    {
        private readonly IConfiguration _configuration;
        private readonly ILogger<DLPService> _logger;
        private readonly List<DLPPolicy> _policies;

        public DLPService(IConfiguration configuration, ILogger<DLPService> logger)
        {
            _configuration = configuration;
            _logger = logger;
            _policies = new List<DLPPolicy>();
            LoadPolicies();
        }

        public async Task<DLPScanResult> ScanContentAsync(string content)
        {
            _logger.LogInformation("Starting content scanning process");
            var result = new DLPScanResult();

            foreach (var policy in _policies)
            {
                foreach (var rule in policy.Rules)
                {
                    var matches = MatchContent(content, rule);
                    foreach (var match in matches)
                    {
                        result.Matches.Add(new DLPMatch
                        {
                            PolicyId = policy.Id,
                            RuleId = rule.Id,
                            MatchedContent = match.Value,
                            StartIndex = match.Index,
                            Length = match.Length
                        });
                    }
                }
            }

            _logger.LogInformation("Content scanning process completed");
            return result;
        }

        public async Task<string> ApplyDLPPolicyAsync(string content, DLPPolicy policy)
        {
            _logger.LogInformation($"Starting policy application process for policy {policy.Id}");

            foreach (var rule in policy.Rules)
            {
                var matches = MatchContent(content, rule);
                foreach (var match in matches)
                {
                    content = ApplyAction(content, match, rule.Action);
                }
            }

            _logger.LogInformation($"Policy application process completed for policy {policy.Id}");
            return content;
        }

        public async Task<bool> AddPolicyAsync(DLPPolicy policy)
        {
            if (policy == null || string.IsNullOrEmpty(policy.Id) || policy.Rules == null || policy.Rules.Count == 0)
            {
                _logger.LogError("Invalid policy provided");
                return false;
            }

            _policies.Add(policy);
            _logger.LogInformation($"New policy added: {policy.Id}");
            return true;
        }

        public async Task<bool> RemovePolicyAsync(string policyId)
        {
            var policy = _policies.Find(p => p.Id == policyId);
            if (policy == null)
            {
                _logger.LogError($"Policy not found: {policyId}");
                return false;
            }

            _policies.Remove(policy);
            _logger.LogInformation($"Policy removed: {policyId}");
            return true;
        }

        private void LoadPolicies()
        {
            var policiesConfig = _configuration.GetSection("DLPPolicies").Get<List<DLPPolicy>>();
            if (policiesConfig != null)
            {
                foreach (var policy in policiesConfig)
                {
                    _policies.Add(policy);
                }
            }
            _logger.LogInformation($"Loaded {_policies.Count} DLP policies from configuration");
        }

        private IEnumerable<Match> MatchContent(string content, DLPRule rule)
        {
            var regex = new Regex(rule.Pattern, RegexOptions.IgnoreCase);
            return regex.Matches(content);
        }

        private string ApplyAction(string content, Match match, DLPAction action)
        {
            switch (action)
            {
                case DLPAction.Redact:
                    return content.Remove(match.Index, match.Length).Insert(match.Index, new string('*', match.Length));
                case DLPAction.Encrypt:
                    // TODO: Implement encryption logic
                    _logger.LogWarning("Encryption action not implemented");
                    return content;
                case DLPAction.Alert:
                    _logger.LogWarning($"DLP Alert: Sensitive content detected - {match.Value}");
                    return content;
                default:
                    _logger.LogError($"Unknown DLP action: {action}");
                    return content;
            }
        }
    }
}

// Human tasks:
// TODO: Implement encryption logic in the ApplyAction method for the Encrypt action