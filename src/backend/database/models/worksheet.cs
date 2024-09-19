using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;

namespace Backend.Database.Models
{
    /// <summary>
    /// Represents a worksheet in an Excel workbook
    /// </summary>
    [Table("Worksheets")]
    public class Worksheet
    {
        /// <summary>
        /// Unique identifier for the worksheet
        /// </summary>
        [Key]
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public int Id { get; set; }

        /// <summary>
        /// Name of the worksheet
        /// </summary>
        [Required]
        [StringLength(100)]
        public string Name { get; set; }

        /// <summary>
        /// ID of the workbook this worksheet belongs to
        /// </summary>
        [Required]
        [ForeignKey("Workbook")]
        public int WorkbookId { get; set; }

        /// <summary>
        /// Navigation property to the parent workbook
        /// </summary>
        public Workbook Workbook { get; set; }

        /// <summary>
        /// Index of the worksheet within the workbook
        /// </summary>
        [Required]
        public int Index { get; set; }

        /// <summary>
        /// Indicates whether the worksheet is visible
        /// </summary>
        [Required]
        public bool IsVisible { get; set; }

        /// <summary>
        /// Timestamp when the worksheet was created
        /// </summary>
        [Required]
        public DateTime CreatedAt { get; set; }

        /// <summary>
        /// Timestamp when the worksheet was last modified
        /// </summary>
        [Required]
        public DateTime ModifiedAt { get; set; }

        /// <summary>
        /// Collection of cells in the worksheet
        /// </summary>
        public ICollection<Cell> Cells { get; set; }

        /// <summary>
        /// Collection of charts in the worksheet
        /// </summary>
        public ICollection<Chart> Charts { get; set; }

        /// <summary>
        /// Collection of pivot tables in the worksheet
        /// </summary>
        public ICollection<PivotTable> PivotTables { get; set; }

        /// <summary>
        /// Retrieves a cell by its row and column indices
        /// </summary>
        /// <param name="row">The row index of the cell</param>
        /// <param name="column">The column index of the cell</param>
        /// <returns>The cell at the specified row and column, or null if not found</returns>
        public Cell GetCell(int row, int column)
        {
            return Cells.FirstOrDefault(c => c.Row == row && c.Column == column);
        }

        /// <summary>
        /// Adds a new cell to the worksheet
        /// </summary>
        /// <param name="row">The row index of the new cell</param>
        /// <param name="column">The column index of the new cell</param>
        /// <param name="value">The value of the new cell</param>
        /// <returns>The newly created cell</returns>
        public Cell AddCell(int row, int column, string value)
        {
            var cell = new Cell { Row = row, Column = column, Value = value, Worksheet = this };
            Cells.Add(cell);
            return cell;
        }

        /// <summary>
        /// Adds a new chart to the worksheet
        /// </summary>
        /// <param name="chart">The chart to add to the worksheet</param>
        public void AddChart(Chart chart)
        {
            chart.Worksheet = this;
            Charts.Add(chart);
        }

        /// <summary>
        /// Adds a new pivot table to the worksheet
        /// </summary>
        /// <param name="pivotTable">The pivot table to add to the worksheet</param>
        public void AddPivotTable(PivotTable pivotTable)
        {
            pivotTable.Worksheet = this;
            PivotTables.Add(pivotTable);
        }
    }
}