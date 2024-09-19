using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace ExcelV2.Backend.Database.Models
{
    /// <summary>
    /// Represents a user in the Excel application
    /// </summary>
    [Table("Users")]
    public class User
    {
        /// <summary>
        /// Unique identifier for the user
        /// </summary>
        [Key]
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public int Id { get; set; }

        /// <summary>
        /// User's username for login
        /// </summary>
        [Required]
        [StringLength(50, MinimumLength = 3)]
        public string Username { get; set; }

        /// <summary>
        /// User's email address
        /// </summary>
        [Required]
        [EmailAddress]
        [StringLength(100)]
        public string Email { get; set; }

        /// <summary>
        /// Hashed password of the user
        /// </summary>
        [Required]
        public string PasswordHash { get; set; }

        /// <summary>
        /// User's first name
        /// </summary>
        [StringLength(50)]
        public string FirstName { get; set; }

        /// <summary>
        /// User's last name
        /// </summary>
        [StringLength(50)]
        public string LastName { get; set; }

        /// <summary>
        /// Timestamp of the user's last login
        /// </summary>
        public DateTime LastLogin { get; set; }

        /// <summary>
        /// Timestamp when the user account was created
        /// </summary>
        [Required]
        public DateTime CreatedAt { get; set; }

        /// <summary>
        /// Timestamp when the user account was last updated
        /// </summary>
        public DateTime? UpdatedAt { get; set; }

        /// <summary>
        /// Indicates whether the user account is active
        /// </summary>
        [Required]
        public bool IsActive { get; set; }

        /// <summary>
        /// Collection of workbooks owned by the user
        /// </summary>
        public ICollection<Workbook> Workbooks { get; set; }

        /// <summary>
        /// Gets the full name of the user
        /// </summary>
        /// <returns>The full name of the user</returns>
        public string FullName()
        {
            return $"{FirstName} {LastName}".Trim();
        }
    }
}