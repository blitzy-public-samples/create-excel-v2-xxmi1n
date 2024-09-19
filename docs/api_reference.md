# Excel API Reference

Version: 1.0.0
Base URL: https://api.excel.example.com/v1

## Authentication

All API requests must be authenticated using OAuth 2.0 bearer tokens.

## Endpoints

### Workbooks

Endpoints for managing workbooks

#### List all workbooks

```
GET /workbooks
```

Parameters:
- `page` (integer): Page number for pagination
- `per_page` (integer): Number of items per page

Responses:
- 200: Successful response
  - Schema: Array of Workbook objects

#### Create a new workbook

```
POST /workbooks
```

Request Body:
- Content-Type: application/json
- Schema: WorkbookCreateRequest

Responses:
- 201: Workbook created successfully
  - Schema: Workbook object

### Worksheets

Endpoints for managing worksheets within a workbook

#### List all worksheets in a workbook

```
GET /workbooks/{workbook_id}/worksheets
```

Parameters:
- `workbook_id` (string): ID of the workbook

Responses:
- 200: Successful response
  - Schema: Array of Worksheet objects

### Cells

Endpoints for manipulating cell data

#### Get the value of a specific cell

```
GET /workbooks/{workbook_id}/worksheets/{worksheet_id}/cells/{cell_reference}
```

Parameters:
- `workbook_id` (string): ID of the workbook
- `worksheet_id` (string): ID of the worksheet
- `cell_reference` (string): Cell reference (e.g., 'A1')

Responses:
- 200: Successful response
  - Schema: Cell object

## Data Models

### Workbook

Properties:
- `id` (string): Unique identifier for the workbook
- `name` (string): Name of the workbook
- `created_at` (datetime): Timestamp of workbook creation
- `modified_at` (datetime): Timestamp of last modification

### Worksheet

Properties:
- `id` (string): Unique identifier for the worksheet
- `name` (string): Name of the worksheet
- `index` (integer): Index of the worksheet in the workbook

### Cell

Properties:
- `reference` (string): Cell reference (e.g., 'A1')
- `value` (any): Value of the cell
- `formula` (string): Formula of the cell, if applicable

## Error Responses

- 400: Bad Request - The request was invalid or cannot be served
- 401: Unauthorized - The request requires authentication
- 403: Forbidden - The server understood the request but refuses to authorize it
- 404: Not Found - The requested resource could not be found
- 429: Too Many Requests - The user has sent too many requests in a given amount of time
- 500: Internal Server Error - The server encountered an unexpected condition that prevented it from fulfilling the request

## Rate Limiting

API requests are limited to 60 requests per minute per API key

Headers:
- `X-RateLimit-Limit`: The maximum number of requests you're permitted to make per minute
- `X-RateLimit-Remaining`: The number of requests remaining in the current rate limit window
- `X-RateLimit-Reset`: The time at which the current rate limit window resets in UTC epoch seconds

## Changelog

### Version 1.0.0 (2023-06-01)
- Initial release of the Excel API