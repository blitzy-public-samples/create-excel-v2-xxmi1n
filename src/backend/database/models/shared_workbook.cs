using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace create_excel_v2_xxmi1n.Backend.Database.Models
{
    /// <summary>
    /// Represents a shared workbook for collaboration in Excel
    /// </summary>
    [Table("SharedWorkbooks")]
    public class SharedWorkbook
    {
        /// <summary>
        /// Unique identifier for the shared workbook
        /// </summary>
        [Key]
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public int Id { get; set; }

        /// <summary>
        /// ID of the workbook that is shared
        /// </summary>
        [Required]
        [ForeignKey("Workbook")]
        public int WorkbookId { get; set; }

        /// <summary>
        /// Navigation property to the associated workbook
        /// </summary>
        public Workbook Workbook { get; set; }

        /// <summary>
        /// Timestamp when the workbook was first shared
        /// </summary>
        [Required]
        public DateTime SharedAt { get; set; }

        /// <summary>
        /// User who shared the workbook
        /// </summary>
        [Required]
        [StringLength(255)]
        public string SharedBy { get; set; }

        /// <summary>
        /// Indicates if changes should be tracked
        /// </summary>
        [Required]
        public bool TrackChanges { get; set; }

        /// <summary>
        /// Interval (in minutes) for merging changes
        /// </summary>
        public int MergeInterval { get; set; }

        /// <summary>
        /// Collection of users collaborating on this workbook
        /// </summary>
        public ICollection<User> Collaborators { get; set; }

        /// <summary>
        /// Collection of revisions made to the shared workbook
        /// </summary>
        public ICollection<Revision> Revisions { get; set; }

        /// <summary>
        /// Adds a collaborator to the shared workbook
        /// </summary>
        /// <param name="user">The user to add as a collaborator</param>
        /// <returns>True if the collaborator was added successfully, false otherwise</returns>
        public bool AddCollaborator(User user)
        {
            if (!Collaborators.Contains(user))
            {
                Collaborators.Add(user);
                return true;
            }
            return false;
        }

        /// <summary>
        /// Removes a collaborator from the shared workbook
        /// </summary>
        /// <param name="user">The user to remove as a collaborator</param>
        /// <returns>True if the collaborator was removed successfully, false otherwise</returns>
        public bool RemoveCollaborator(User user)
        {
            return Collaborators.Remove(user);
        }

        /// <summary>
        /// Adds a new revision to the shared workbook
        /// </summary>
        /// <param name="revision">The revision to add</param>
        public void AddRevision(Revision revision)
        {
            Revisions.Add(revision);
        }

        /// <summary>
        /// Toggles the track changes setting
        /// </summary>
        /// <returns>The new track changes status</returns>
        public bool ToggleTrackChanges()
        {
            TrackChanges = !TrackChanges;
            return TrackChanges;
        }
    }
}