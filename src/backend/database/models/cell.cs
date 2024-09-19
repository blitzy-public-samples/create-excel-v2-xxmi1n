using System;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace create_excel_v2_xxmi1n.Backend.Database.Models
{
    /// <summary>
    /// Represents a cell in an Excel worksheet
    /// </summary>
    [Table("Cells")]
    public class Cell
    {
        /// <summary>
        /// Unique identifier for the cell
        /// </summary>
        [Key]
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public int Id { get; set; }

        /// <summary>
        /// ID of the worksheet this cell belongs to
        /// </summary>
        [Required]
        [ForeignKey("Worksheet")]
        public int WorksheetId { get; set; }

        /// <summary>
        /// Navigation property to the parent worksheet
        /// </summary>
        public Worksheet Worksheet { get; set; }

        /// <summary>
        /// Row index of the cell
        /// </summary>
        [Required]
        public int Row { get; set; }

        /// <summary>
        /// Column index of the cell
        /// </summary>
        [Required]
        public int Column { get; set; }

        /// <summary>
        /// The value stored in the cell
        /// </summary>
        public string Value { get; set; }

        /// <summary>
        /// The data type of the cell value (e.g., string, number, date)
        /// </summary>
        public string DataType { get; set; }

        /// <summary>
        /// ID of the style applied to this cell
        /// </summary>
        [ForeignKey("Style")]
        public int? StyleId { get; set; }

        /// <summary>
        /// Navigation property to the cell's style
        /// </summary>
        public Style Style { get; set; }

        /// <summary>
        /// ID of the formula associated with this cell
        /// </summary>
        [ForeignKey("Formula")]
        public int? FormulaId { get; set; }

        /// <summary>
        /// Navigation property to the cell's formula
        /// </summary>
        public Formula Formula { get; set; }

        /// <summary>
        /// Timestamp when the cell was created
        /// </summary>
        [Required]
        public DateTime CreatedAt { get; set; }

        /// <summary>
        /// Timestamp when the cell was last modified
        /// </summary>
        [Required]
        public DateTime ModifiedAt { get; set; }

        /// <summary>
        /// Gets the A1-style reference for this cell
        /// </summary>
        /// <returns>The A1-style reference (e.g., 'A1', 'B2')</returns>
        public string GetCellReference()
        {
            return $"{(char)('A' + Column - 1)}{Row}";
        }

        /// <summary>
        /// Sets the value of the cell and updates the data type
        /// </summary>
        /// <param name="value">The value to set</param>
        public void SetValue(object value)
        {
            Value = value?.ToString();
            DataType = DetermineDataType(value);
            ModifiedAt = DateTime.UtcNow;
        }

        /// <summary>
        /// Determines the data type of the given value
        /// </summary>
        /// <param name="value">The value to determine the type of</param>
        /// <returns>The determined data type</returns>
        private string DetermineDataType(object value)
        {
            if (value == null) return "null";
            if (value is string) return "string";
            if (value is int || value is double || value is decimal) return "number";
            if (value is DateTime) return "date";
            if (value is bool) return "boolean";
            return "unknown";
        }
    }
}