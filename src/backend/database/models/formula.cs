using System;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace create_excel_v2_xxmi1n.Backend.Database.Models
{
    /// <summary>
    /// Represents a formula in an Excel cell
    /// </summary>
    [Table("Formulas")]
    public class Formula
    {
        /// <summary>
        /// Unique identifier for the formula
        /// </summary>
        [Key]
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public int Id { get; set; }

        /// <summary>
        /// ID of the cell this formula belongs to
        /// </summary>
        [Required]
        [ForeignKey("Cell")]
        public int CellId { get; set; }

        /// <summary>
        /// Navigation property to the parent cell
        /// </summary>
        public Cell Cell { get; set; }

        /// <summary>
        /// The formula expression
        /// </summary>
        [Required]
        [StringLength(1000)]
        public string Expression { get; set; }

        /// <summary>
        /// The compiled or optimized version of the formula expression
        /// </summary>
        public string CompiledExpression { get; set; }

        /// <summary>
        /// Timestamp when the formula was created
        /// </summary>
        [Required]
        public DateTime CreatedAt { get; set; }

        /// <summary>
        /// Timestamp when the formula was last modified
        /// </summary>
        [Required]
        public DateTime ModifiedAt { get; set; }

        /// <summary>
        /// Indicates whether the formula is valid
        /// </summary>
        [Required]
        public bool IsValid { get; set; }

        /// <summary>
        /// Error message if the formula is invalid
        /// </summary>
        public string ErrorMessage { get; set; }

        /// <summary>
        /// Validates the formula expression
        /// </summary>
        /// <returns>True if the formula is valid, false otherwise</returns>
        public bool Validate()
        {
            // TODO: Implement formula validation logic
            IsValid = true; // Placeholder
            ErrorMessage = null;
            return IsValid;
        }

        /// <summary>
        /// Compiles or optimizes the formula expression
        /// </summary>
        public void Compile()
        {
            if (Validate())
            {
                // TODO: Implement formula compilation logic
                CompiledExpression = Expression; // Placeholder
            }
            else
            {
                CompiledExpression = null;
            }
        }

        /// <summary>
        /// Updates the formula expression and revalidates
        /// </summary>
        /// <param name="newExpression">The new formula expression</param>
        /// <returns>True if the update was successful, false otherwise</returns>
        public bool UpdateExpression(string newExpression)
        {
            Expression = newExpression;
            ModifiedAt = DateTime.UtcNow;
            return Validate();
        }
    }
}