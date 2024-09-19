using System;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using Newtonsoft.Json;

namespace Backend.Database.Models
{
    /// <summary>
    /// Represents a revision in a shared workbook
    /// </summary>
    [Table("Revisions")]
    public class Revision
    {
        /// <summary>
        /// Unique identifier for the revision
        /// </summary>
        [Key]
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public int Id { get; set; }

        /// <summary>
        /// ID of the shared workbook this revision belongs to
        /// </summary>
        [Required]
        [ForeignKey("SharedWorkbook")]
        public int SharedWorkbookId { get; set; }

        /// <summary>
        /// Navigation property to the associated shared workbook
        /// </summary>
        public SharedWorkbook SharedWorkbook { get; set; }

        /// <summary>
        /// ID of the user who made the revision
        /// </summary>
        [Required]
        [ForeignKey("User")]
        public int UserId { get; set; }

        /// <summary>
        /// Navigation property to the user who made the revision
        /// </summary>
        public User User { get; set; }

        /// <summary>
        /// Timestamp when the revision was made
        /// </summary>
        [Required]
        public DateTime Timestamp { get; set; }

        /// <summary>
        /// Type of change (e.g., 'CellEdit', 'FormulaChange', 'StyleChange')
        /// </summary>
        [Required]
        [StringLength(50)]
        public string ChangeType { get; set; }

        /// <summary>
        /// Name of the worksheet where the change occurred
        /// </summary>
        [Required]
        [StringLength(255)]
        public string SheetName { get; set; }

        /// <summary>
        /// Cell reference for the change (if applicable)
        /// </summary>
        [StringLength(20)]
        public string CellReference { get; set; }

        /// <summary>
        /// Previous value before the change
        /// </summary>
        public string OldValue { get; set; }

        /// <summary>
        /// New value after the change
        /// </summary>
        public string NewValue { get; set; }

        /// <summary>
        /// JSON string containing additional information about the change
        /// </summary>
        public string AdditionalInfo { get; set; }

        /// <summary>
        /// Sets additional information for the revision
        /// </summary>
        /// <param name="info">The object containing additional information</param>
        public void SetAdditionalInfo(object info)
        {
            AdditionalInfo = JsonConvert.SerializeObject(info);
        }

        /// <summary>
        /// Gets additional information as a specific type
        /// </summary>
        /// <typeparam name="T">The type to deserialize the additional information into</typeparam>
        /// <returns>The deserialized additional information</returns>
        public T GetAdditionalInfo<T>()
        {
            return JsonConvert.DeserializeObject<T>(AdditionalInfo);
        }

        /// <summary>
        /// Applies the revision to a workbook (placeholder method)
        /// </summary>
        /// <param name="workbook">The workbook to apply the revision to</param>
        /// <returns>True if the revision was applied successfully, false otherwise</returns>
        public bool ApplyRevision(Workbook workbook)
        {
            // TODO: Implement logic to apply the revision to the workbook
            // This will depend on the ChangeType and the specific change made
            return true;
        }
    }
}