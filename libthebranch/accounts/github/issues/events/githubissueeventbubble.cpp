#include "githubissueeventbubble.h"
#include "ui_githubissueeventbubble.h"

#include "../githubissueevent.h"

struct GitHubIssueEventBubblePrivate {
        GitHubIssueEventPtr item;
};

GitHubIssueEventBubble::GitHubIssueEventBubble(GitHubIssueEventPtr item, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GitHubIssueEventBubble) {
    ui->setupUi(this);

    d = new GitHubIssueEventBubblePrivate();
    d->item = item;
    connect(item.data(), &GitHubItem::updated, this, &GitHubIssueEventBubble::updateData);
    this->updateData();
}

GitHubIssueEventBubble::~GitHubIssueEventBubble() {
    delete ui;
}

void GitHubIssueEventBubble::updateData() {
    ui->descriptionLabel->setText(d->item->event());
}
