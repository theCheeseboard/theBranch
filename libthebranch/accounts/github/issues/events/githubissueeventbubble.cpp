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
    } else if (event == "unassigned") {
        auto assignee = d->item->account()->itemDb()->update<GitHubUser>(d->item->account(), d->item->remote(), d->item->eventProperty("assignee").toJsonObject());
        if (assignee == d->item->actor()) {
            ui->descriptionLabel->setText(tr("Unassigned to %1 by themselves").arg(embolden(assignee->login())));
        } else {
            ui->descriptionLabel->setText(tr("Unassigned to %1 by %2").arg(embolden(assignee->login()), embolden(d->item->actor()->login())));
        }
    } else if (event == "locked") {
        ui->descriptionLabel->setText(tr("Locked by %1").arg(embolden(d->item->actor()->login())));
    } else if (event == "unlocked") {
        ui->descriptionLabel->setText(tr("Unlocked by %1").arg(embolden(d->item->actor()->login())));
    } else if (event == "head_ref_deleted") {
        ui->descriptionLabel->setText(tr("Source branch deleted by %1").arg(embolden(d->item->actor()->login())));
    } else if (event == "head_ref_restored") {
        ui->descriptionLabel->setText(tr("Source branch restored by %1").arg(embolden(d->item->actor()->login())));
    } else if (event == "head_ref_force_pushed") {
        ui->descriptionLabel->setText(tr("Source branch force pushed to by %1").arg(embolden(d->item->actor()->login())));
    } else if (event == "pinned") {
        ui->descriptionLabel->setText(tr("Pinned by %1").arg(embolden(d->item->actor()->login())));
    } else if (event == "unpinned") {
        ui->descriptionLabel->setText(tr("Unpinned by %1").arg(embolden(d->item->actor()->login())));
    } else if (event == "ready_for_review") {
        ui->descriptionLabel->setText(tr("Marked ready for review by %1").arg(embolden(d->item->actor()->login())));
    } else if (event == "convert_to_draft") {
        ui->descriptionLabel->setText(tr("Converted to draft by %1").arg(embolden(d->item->actor()->login())));
    } else if (event == "deployed") {
        ui->descriptionLabel->setText(tr("Deployed by %1").arg(embolden(d->item->actor()->login())));
    } else {
        ui->descriptionLabel->setText(d->item->event());
    }
}

void GitHubIssueEventBubble::on_descriptionLabel_linkActivated(const QString& link) {
    QUrl url(link);
    if (url.scheme() == "commit") {
    }
}
