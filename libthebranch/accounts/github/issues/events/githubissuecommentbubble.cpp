#include "githubissuecommentbubble.h"
#include "ui_githubissuecommentbubble.h"

#include "../githubissue.h"
#include "../githubissuecommentevent.h"

struct GitHubIssueCommentBubblePrivate {
        GitHubItemPtr item;
};

GitHubIssueCommentBubble::GitHubIssueCommentBubble(GitHubItemPtr item, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GitHubIssueCommentBubble) {
    ui->setupUi(this);

    d = new GitHubIssueCommentBubblePrivate();
    d->item = item;
    connect(item.data(), &GitHubItem::updated, this, &GitHubIssueCommentBubble::updateData);
    this->updateData();
}

GitHubIssueCommentBubble::~GitHubIssueCommentBubble() {
    delete ui;
    delete d;
}

void GitHubIssueCommentBubble::updateData() {
    if (auto issue = d->item.objectCast<GitHubIssue>()) {
        ui->commentText->setText(issue->body());
    } else if (auto event = d->item.objectCast<GitHubIssueCommentEvent>()) {
        ui->commentText->setText(event->body());
    }
}
