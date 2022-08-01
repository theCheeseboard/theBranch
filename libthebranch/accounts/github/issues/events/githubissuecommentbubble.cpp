#include "githubissuecommentbubble.h"
#include "ui_githubissuecommentbubble.h"

#include "../../users/githubuser.h"
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

    auto titlePal = ui->titleLabel->palette();
    titlePal.setColor(QPalette::WindowText, ui->titleLabel->palette().color(QPalette::Window));
    titlePal.setColor(QPalette::Window, ui->titleLabel->palette().color(QPalette::WindowText));
    ui->titleLabel->setPalette(titlePal);
}

GitHubIssueCommentBubble::~GitHubIssueCommentBubble() {
    delete ui;
    delete d;
}

void GitHubIssueCommentBubble::updateData() {
    GitHubUserPtr user;
    if (auto issue = d->item.objectCast<GitHubIssue>()) {
        ui->titleLabel->setVisible(false);
        ui->commentText->setText(issue->body());
        user = issue->user();
    } else if (auto event = d->item.objectCast<GitHubIssueCommentEvent>()) {
        if (event->event() == "commented") {
            ui->titleLabel->setVisible(false);
        } else if (event->event() == "reviewed") {
            auto state = event->eventProperty("state").toString();
            if (state == "commented") {
                ui->titleLabel->setText(tr("Review Comment"));
            } else if (state == "changes_requested") {
                ui->titleLabel->setText(tr("Requested Changes"));
            } else if (state == "approved") {
                ui->titleLabel->setText(tr("Approved Pull Request"));
            }
        }
        ui->commentText->setText(event->body());
        user = event->user();
        ui->titleLabel->setText(tr("Review Comment"));
    }

    if (user) {
        ui->extraData->setText(user->login());
    }
}
