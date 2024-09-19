using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace Backend.Database.Models
{
    /// <summary>
    /// Represents an Excel workbook in the application
    /// </summary>
    [Table("Workbooks")]
    public class Workbook
    {
        /// <summary>
        /// Unique identifier for the workbook
        /// </summary>
        [Key]
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public int Id { get; set; }

        /// <summary>
        /// Name of the workbook
        /// </summary>
        [Required]
        [StringLength(255)]
        public string Name { get; set; }

        /// <summary>
        /// Optional description of the workbook
        /// </summary>
        [StringLength(1000)]
        public string Description { get; set; }

        /// <summary>
        /// Timestamp when the workbook was created
        /// </summary>
        [Required]
        public DateTime CreatedAt { get; set; }

        /// <summary>
        /// Timestamp when the workbook was last modified
        /// </summary>
        [Required]
        public DateTime ModifiedAt { get; set; }

        /// <summary>
        /// ID of the user who owns the workbook
        /// </summary>
        [Required]
        [ForeignKey("User")]
        public int OwnerId { get; set; }

        /// <summary>
        /// Navigation property to the user who owns the workbook
        /// </summary>
        public User Owner { get; set; }

        /// <summary>
        /// Collection of worksheets in the workbook
        /// </summary>
        public ICollection<Worksheet> Worksheets { get; set; }

        /// <summary>
        /// Collection of named ranges in the workbook
        /// </summary>
        public ICollection<NamedRange> NamedRanges { get; set; }

        /// <summary>
        /// Collection of macros in the workbook
        /// </summary>
        public ICollection<Macro> Macros { get; set; }

        /// <summary>
        /// Indicates whether the workbook is shared for collaboration
        /// </summary>
        [Required]
        public bool IsShared { get; set; }

        /// <summary>
        /// Navigation property to the shared workbook details, if applicable
        /// </summary>
        public SharedWorkbook SharedWorkbook { get; set; }

        /// <summary>
        /// Adds a new worksheet to the workbook
        /// </summary>
        /// <param name="name">The name of the new worksheet</param>
        /// <returns>The newly created worksheet</returns>
        public Worksheet AddWorksheet(string name)
        {
            var worksheet = new Worksheet { Name = name, Workbook = this };
            Worksheets.Add(worksheet);
            return worksheet;
        }

        /// <summary>
        /// Retrieves a worksheet by its name
        /// </summary>
        /// <param name="name">The name of the worksheet to retrieve</param>
        /// <returns>The worksheet with the specified name, or null if not found</returns>
        public Worksheet GetWorksheetByName(string name)
        {
            return Worksheets.FirstOrDefault(w => w.Name == name);
        }
    }
}