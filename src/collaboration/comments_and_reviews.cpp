#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <chrono>
#include "excel_types.h"
#include "data_manager.h"
#include "user_manager.h"
#include "notification_manager.h"
#include "comments_and_reviews.h"

const int MAX_COMMENT_LENGTH = 1000;
const int MAX_COMMENTS_PER_CELL = 50;

CommentsAndReviews::CommentsAndReviews(std::shared_ptr<DataManager> dataManager,
                                       std::shared_ptr<UserManager> userManager,
                                       std::shared_ptr<NotificationManager> notificationManager)
    : m_dataManager(dataManager),
      m_userManager(userManager),
      m_notificationManager(notificationManager) {
    // Initialize m_comments and m_reviews as empty unordered_maps
    m_comments = std::unordered_map<std::string, std::vector<std::shared_ptr<Comment>>>();
    m_reviews = std::unordered_map<std::string, std::shared_ptr<Review>>();
}

std::string CommentsAndReviews::AddComment(std::shared_ptr<Workbook> workbook,
                                           const CellReference& cellRef,
                                           const std::string& commentText,
                                           const std::string& userId) {
    // Validate the comment
    if (!ValidateComment(commentText)) {
        return "";
    }

    // Generate a unique comment ID
    std::string commentId = GenerateCommentId();

    // Create a new Comment object
    auto comment = std::make_shared<Comment>();
    comment->id = commentId;
    comment->text = commentText;
    comment->userId = userId;
    comment->userName = m_userManager->GetUserName(userId);
    comment->timestamp = std::chrono::system_clock::now();

    // Add the comment to m_comments for the specific workbook and cell
    std::string key = workbook->GetId() + "_" + cellRef.ToString();
    m_comments[key].push_back(comment);

    // Notify relevant users
    m_notificationManager->SendCommentNotification(workbook, cellRef, userId, commentText);

    return commentId;
}

bool CommentsAndReviews::EditComment(const std::string& commentId,
                                     const std::string& newCommentText,
                                     const std::string& userId) {
    // Find the comment with the given commentId
    for (auto& commentList : m_comments) {
        for (auto& comment : commentList.second) {
            if (comment->id == commentId) {
                // Validate the new comment text
                if (!ValidateComment(newCommentText)) {
                    return false;
                }

                // Update the comment text and modification timestamp
                comment->text = newCommentText;
                comment->lastModified = std::chrono::system_clock::now();

                // Notify relevant users
                m_notificationManager->SendCommentUpdateNotification(commentId, userId, newCommentText);

                return true;
            }
        }
    }

    return false;
}

bool CommentsAndReviews::DeleteComment(const std::string& commentId, const std::string& userId) {
    // Find the comment with the given commentId
    for (auto& commentList : m_comments) {
        auto it = std::find_if(commentList.second.begin(), commentList.second.end(),
                               [&](const std::shared_ptr<Comment>& comment) {
                                   return comment->id == commentId;
                               });

        if (it != commentList.second.end()) {
            // Check if the user has permission to delete the comment
            if ((*it)->userId != userId) {
                return false;
            }

            // Remove the comment from m_comments
            commentList.second.erase(it);

            // Notify relevant users
            m_notificationManager->SendCommentDeleteNotification(commentId, userId);

            return true;
        }
    }

    return false;
}

std::string CommentsAndReviews::StartReview(std::shared_ptr<Workbook> workbook,
                                            const std::vector<std::string>& reviewerIds,
                                            const std::string& initiatorId) {
    // Generate a unique review ID
    std::string reviewId = GenerateReviewId();

    // Create a new Review object
    auto review = std::make_shared<Review>();
    review->id = reviewId;
    review->workbookId = workbook->GetId();
    review->initiatorId = initiatorId;
    review->reviewerIds = reviewerIds;
    review->status = ReviewStatus::InProgress;
    review->startTime = std::chrono::system_clock::now();

    // Add the review to m_reviews
    m_reviews[reviewId] = review;

    // Notify reviewers
    m_notificationManager->SendReviewStartNotification(workbook, reviewerIds, initiatorId);

    return reviewId;
}

bool CommentsAndReviews::SubmitReviewFeedback(const std::string& reviewId,
                                              const std::string& reviewerId,
                                              const std::string& feedback,
                                              ReviewStatus status) {
    // Find the review with the given reviewId
    auto it = m_reviews.find(reviewId);
    if (it == m_reviews.end()) {
        return false;
    }

    auto& review = it->second;

    // Add the feedback to the review object
    review->feedback[reviewerId] = feedback;
    review->reviewerStatus[reviewerId] = status;

    // Update the review status if all reviewers have submitted feedback
    if (review->feedback.size() == review->reviewerIds.size()) {
        review->status = ReviewStatus::Completed;
    }

    // Notify the review initiator
    m_notificationManager->SendReviewFeedbackNotification(reviewId, reviewerId, feedback, status);

    return true;
}

bool ValidateComment(const std::string& commentText) {
    // Check if the comment text is not empty
    if (commentText.empty()) {
        return false;
    }

    // Ensure the comment length does not exceed MAX_COMMENT_LENGTH
    if (commentText.length() > MAX_COMMENT_LENGTH) {
        return false;
    }

    // Validate the comment text for any prohibited content or formatting
    // (Implement additional checks as needed)

    return true;
}

std::string GenerateCommentId() {
    // Generate a UUID or use a combination of timestamp and random characters
    // This is a simplified implementation and should be replaced with a more robust method
    static int counter = 0;
    return "COMMENT_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + "_" + std::to_string(++counter);
}

std::string GenerateReviewId() {
    // Generate a UUID or use a combination of timestamp and random characters
    // This is a simplified implementation and should be replaced with a more robust method
    static int counter = 0;
    return "REVIEW_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + "_" + std::to_string(++counter);
}