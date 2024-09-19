using System;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace Backend.Database.Models
{
    /// <summary>
    /// Represents a named range in an Excel workbook
    /// </summary>
    [Table("NamedRanges")]
    public class NamedRange
    {
        /// <summary>
        /// Unique identifier for the named range
        /// </summary>
        [Key]
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public int Id { get; set; }

        /// <summary>
        /// ID of the workbook this named range belongs to
        /// </summary>
        [Required]
        [ForeignKey("Workbook")]
        public int WorkbookId { get; set; }

        /// <summary>
        /// Navigation property to the parent workbook
        /// </summary>
        public Workbook Workbook { get; set; }

        /// <summary>
        /// Name of the named range
        /// </summary>
        [Required]
        [StringLength(255)]
        public string Name { get; set; }

        /// <summary>
        /// String representation of the cell range reference
        /// </summary>
        [Required]
        [StringLength(1000)]
        public string ReferenceString { get; set; }

        /// <summary>
        /// ID of the worksheet this named range is scoped to (null if workbook-scoped)
        /// </summary>
        [ForeignKey("Worksheet")]
        public int? WorksheetId { get; set; }

        /// <summary>
        /// Navigation property to the worksheet (if worksheet-scoped)
        /// </summary>
        public Worksheet Worksheet { get; set; }

        /// <summary>
        /// Optional comment describing the named range
        /// </summary>
        [StringLength(1000)]
        public string Comment { get; set; }

        /// <summary>
        /// Timestamp when the named range was created
        /// </summary>
        [Required]
        public DateTime CreatedAt { get; set; }

        /// <summary>
        /// Timestamp when the named range was last modified
        /// </summary>
        [Required]
        public DateTime ModifiedAt { get; set; }

        /// <summary>
        /// Updates the reference string for the named range
        /// </summary>
        /// <param name="newReferenceString">The new reference string to set</param>
        public void UpdateReference(string newReferenceString)
        {
            ReferenceString = newReferenceString;
            ModifiedAt = DateTime.UtcNow;
        }

        /// <summary>
        /// Checks if the named range is scoped to the workbook
        /// </summary>
        /// <returns>True if workbook-scoped, false if worksheet-scoped</returns>
        public bool IsWorkbookScoped()
        {
            return WorksheetId == null;
        }

        /// <summary>
        /// Gets the fully qualified name of the named range
        /// </summary>
        /// <returns>The fully qualified name</returns>
        public string GetFullyQualifiedName()
        {
            if (IsWorkbookScoped())
            {
                return Name;
            }
            else
            {
                return $"{Worksheet.Name}!{Name}";
            }
        }
    }
}