using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace create_excel_v2_xxmi1n.Backend.Database.Models
{
    /// <summary>
    /// Represents a style for cells or workbook elements in Excel
    /// </summary>
    [Table("Styles")]
    public class Style
    {
        /// <summary>
        /// Unique identifier for the style
        /// </summary>
        [Key]
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public int Id { get; set; }

        /// <summary>
        /// Name of the style
        /// </summary>
        [Required]
        [StringLength(100)]
        public string Name { get; set; }

        /// <summary>
        /// Font name for the style
        /// </summary>
        [StringLength(50)]
        public string Font { get; set; }

        /// <summary>
        /// Font size for the style
        /// </summary>
        public float? FontSize { get; set; }

        /// <summary>
        /// Font color for the style
        /// </summary>
        [StringLength(20)]
        public string FontColor { get; set; }

        /// <summary>
        /// Indicates if the font is bold
        /// </summary>
        public bool? IsBold { get; set; }

        /// <summary>
        /// Indicates if the font is italic
        /// </summary>
        public bool? IsItalic { get; set; }

        /// <summary>
        /// Indicates if the font is underlined
        /// </summary>
        public bool? IsUnderline { get; set; }

        /// <summary>
        /// Background color for the style
        /// </summary>
        [StringLength(20)]
        public string BackgroundColor { get; set; }

        /// <summary>
        /// Horizontal alignment for the style
        /// </summary>
        [StringLength(20)]
        public string HorizontalAlignment { get; set; }

        /// <summary>
        /// Vertical alignment for the style
        /// </summary>
        [StringLength(20)]
        public string VerticalAlignment { get; set; }

        /// <summary>
        /// Border style for the style
        /// </summary>
        [StringLength(20)]
        public string BorderStyle { get; set; }

        /// <summary>
        /// Border color for the style
        /// </summary>
        [StringLength(20)]
        public string BorderColor { get; set; }

        /// <summary>
        /// Number format for the style
        /// </summary>
        [StringLength(50)]
        public string NumberFormat { get; set; }

        /// <summary>
        /// Timestamp when the style was created
        /// </summary>
        [Required]
        public DateTime CreatedAt { get; set; }

        /// <summary>
        /// Timestamp when the style was last modified
        /// </summary>
        [Required]
        public DateTime ModifiedAt { get; set; }

        /// <summary>
        /// Collection of cells using this style
        /// </summary>
        public ICollection<Cell> Cells { get; set; }

        /// <summary>
        /// Creates a clone of the current style
        /// </summary>
        /// <returns>A new Style object with the same properties</returns>
        public Style Clone()
        {
            return new Style
            {
                Name = this.Name + " (Copy)",
                Font = this.Font,
                FontSize = this.FontSize,
                FontColor = this.FontColor,
                IsBold = this.IsBold,
                IsItalic = this.IsItalic,
                IsUnderline = this.IsUnderline,
                BackgroundColor = this.BackgroundColor,
                HorizontalAlignment = this.HorizontalAlignment,
                VerticalAlignment = this.VerticalAlignment,
                BorderStyle = this.BorderStyle,
                BorderColor = this.BorderColor,
                NumberFormat = this.NumberFormat,
                CreatedAt = DateTime.UtcNow,
                ModifiedAt = DateTime.UtcNow
            };
        }

        /// <summary>
        /// Applies this style to a cell
        /// </summary>
        /// <param name="cell">The cell to apply the style to</param>
        public void ApplyTo(Cell cell)
        {
            cell.Style = this;
            cell.ModifiedAt = DateTime.UtcNow;
        }
    }
}