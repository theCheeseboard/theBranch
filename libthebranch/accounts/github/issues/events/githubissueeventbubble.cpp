#include "githubissueeventbubble.h"
#include "ui_githubissueeventbubble.h"

#include "../../githubaccount.h"
#include "../../githubitemdatabase.h"
#include "../../users/githubuser.h"
#include "../githubissueevent.h"
#include <QJsonValue>

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

QString GitHubIssueEventBubble::embolden(QString text) {
    return QStringLiteral("<b>%1</b>").arg(text);
}

QString GitHubIssueEventBubble::link(QString text, QUrl link) {
    return QStringLiteral("<a href=\"%2\">%1</a>").arg(text, link.toString(QUrl::FullyEncoded));
}

QString GitHubIssueEventBubble::commit(QString id) {
    return link(embolden(id.left(7)), QUrl("commit://" + id));
}

void GitHubIssueEventBubble::updateData() {
    auto event = d->item->event();
    if (event == "closed") {
        ui->descriptionLabel->setText(tr("Closed by %1").arg(embolden(d->item->actor()->login())));
    } else if (event == "reopened") {
        ui->descriptionLabel->setText(tr("Reopened by %1").arg(embolden(d->item->actor()->login())));
    } else if (event == "referenced") {
        ui->descriptionLabel->setText(tr("Referenced in %1").arg(commit(d->item->eventProperty("commit_id").toString())));
    } else if (event == "committed") {
        ui->descriptionLabel->setText(tr("Committed %1").arg(commit(d->item->eventProperty("sha").toString())));
    } else if (event == "assigned") {
        auto assignee = d->item->account()->itemDb()->update<GitHubUser>(d->item->account(), d->item->remote(), d->item->eventProperty("assignee").toJsonObject());
        if (assignee == d->item->actor()) {
            ui->descriptionLabel->setText(tr("Assigned to %1 by themselves").arg(embolden(assignee->login())));
        } else {
            ui->descriptionLabel->setText(tr("Assigned to %1 by %2").arg(embolden(assignee->login()), embolden(d->item->actor()->login())));
        }
    } else {
        ui->descriptionLabel->setText(d->item->event());
    }
}

void GitHubIssueEventBubble::on_descriptionLabel_linkActivated(const QString& link) {
    QUrl url(link);
    if (url.scheme() == "commit") {
    }
}
