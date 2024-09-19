using System;
using System.Security.Cryptography;
using System.IO;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;
using ExcelServices.Interfaces;
using System.Threading.Tasks;

namespace ExcelServices.Services
{
    // Constants for configuration keys
    const string ENCRYPTION_KEY = "EncryptionSettings:Key";
    const string ENCRYPTION_IV = "EncryptionSettings:IV";

    public class DataEncryptionService : IDataEncryptionService
    {
        private readonly IConfiguration _configuration;
        private readonly ILogger<DataEncryptionService> _logger;
        private readonly byte[] _key;
        private readonly byte[] _iv;

        public DataEncryptionService(IConfiguration configuration, ILogger<DataEncryptionService> logger)
        {
            // Initialize configuration and logger
            _configuration = configuration;
            _logger = logger;

            // Retrieve encryption key and IV from configuration
            string keyString = _configuration[ENCRYPTION_KEY];
            string ivString = _configuration[ENCRYPTION_IV];

            // Convert key and IV to byte arrays
            _key = Convert.FromBase64String(keyString);
            _iv = Convert.FromBase64String(ivString);

            // Validate key and IV lengths
            if (_key.Length != 32 || _iv.Length != 16)
            {
                throw new ArgumentException("Invalid encryption key or IV length");
            }
        }

        public async Task<byte[]> EncryptDataAsync(byte[] data)
        {
            _logger.LogInformation("Starting data encryption process");
            var encryptedData = await PerformCryptographyAsync(data, _key, _iv, CryptoOperation.Encrypt);
            _logger.LogInformation("Data encryption process completed");
            return encryptedData;
        }

        public async Task<byte[]> DecryptDataAsync(byte[] encryptedData)
        {
            _logger.LogInformation("Starting data decryption process");
            var decryptedData = await PerformCryptographyAsync(encryptedData, _key, _iv, CryptoOperation.Decrypt);
            _logger.LogInformation("Data decryption process completed");
            return decryptedData;
        }

        private async Task<byte[]> PerformCryptographyAsync(byte[] data, byte[] key, byte[] iv, CryptoOperation operation)
        {
            using (var aes = new AesManaged())
            {
                aes.Key = key;
                aes.IV = iv;

                ICryptoTransform cryptoTransform = operation == CryptoOperation.Encrypt
                    ? aes.CreateEncryptor(aes.Key, aes.IV)
                    : aes.CreateDecryptor(aes.Key, aes.IV);

                using (var memoryStream = new MemoryStream())
                {
                    using (var cryptoStream = new CryptoStream(memoryStream, cryptoTransform, CryptoStreamMode.Write))
                    {
                        await cryptoStream.WriteAsync(data, 0, data.Length);
                        await cryptoStream.FlushFinalBlockAsync();
                    }

                    return memoryStream.ToArray();
                }
            }
        }
    }

    public enum CryptoOperation
    {
        Encrypt,
        Decrypt
    }
}