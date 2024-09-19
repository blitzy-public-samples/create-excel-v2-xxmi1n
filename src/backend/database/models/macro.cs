using System;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace create_excel_v2_xxmi1n.Backend.Database.Models
{
    /// <summary>
    /// Represents a macro in an Excel workbook
    /// </summary>
    [Table("Macros")]
    public class Macro
    {
        /// <summary>
        /// Unique identifier for the macro
        /// </summary>
        [Key]
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public int Id { get; set; }

        /// <summary>
        /// ID of the workbook this macro belongs to
        /// </summary>
        [Required]
        [ForeignKey("Workbook")]
        public int WorkbookId { get; set; }

        /// <summary>
        /// Navigation property to the parent workbook
        /// </summary>
        public Workbook Workbook { get; set; }

        /// <summary>
        /// Name of the macro
        /// </summary>
        [Required]
        [StringLength(255)]
        public string Name { get; set; }

        /// <summary>
        /// Optional description of the macro
        /// </summary>
        [StringLength(1000)]
        public string Description { get; set; }

        /// <summary>
        /// The VBA code of the macro
        /// </summary>
        [Required]
        public string Code { get; set; }

        /// <summary>
        /// The programming language of the macro (e.g., 'VBA')
        /// </summary>
        [Required]
        [StringLength(50)]
        public string Language { get; set; }

        /// <summary>
        /// Indicates if the macro should run automatically
        /// </summary>
        public bool IsAutoRun { get; set; }

        /// <summary>
        /// The scope of the macro (e.g., 'Workbook', 'Worksheet')
        /// </summary>
        [Required]
        [StringLength(50)]
        public string Scope { get; set; }

        /// <summary>
        /// Timestamp when the macro was created
        /// </summary>
        [Required]
        public DateTime CreatedAt { get; set; }

        /// <summary>
        /// Timestamp when the macro was last modified
        /// </summary>
        [Required]
        public DateTime ModifiedAt { get; set; }

        /// <summary>
        /// User who created the macro
        /// </summary>
        [Required]
        [StringLength(255)]
        public string CreatedBy { get; set; }

        /// <summary>
        /// User who last modified the macro
        /// </summary>
        [Required]
        [StringLength(255)]
        public string ModifiedBy { get; set; }

        /// <summary>
        /// Updates the code of the macro
        /// </summary>
        /// <param name="newCode">The new code to be set</param>
        /// <param name="modifiedBy">The user who is modifying the code</param>
        public void UpdateCode(string newCode, string modifiedBy)
        {
            Code = newCode;
            ModifiedAt = DateTime.UtcNow;
            ModifiedBy = modifiedBy;
        }

        /// <summary>
        /// Toggles the auto-run status of the macro
        /// </summary>
        /// <returns>The new auto-run status</returns>
        public bool ToggleAutoRun()
        {
            IsAutoRun = !IsAutoRun;
            ModifiedAt = DateTime.UtcNow;
            return IsAutoRun;
        }

        /// <summary>
        /// Validates the macro (placeholder for actual validation logic)
        /// </summary>
        /// <returns>True if the macro is valid, false otherwise</returns>
        public bool Validate()
        {
            // TODO: Implement actual macro validation logic
            return !string.IsNullOrEmpty(Code);
        }
    }
}